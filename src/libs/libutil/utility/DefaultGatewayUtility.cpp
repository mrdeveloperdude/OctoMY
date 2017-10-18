#include "Utility.hpp"

namespace utility
{
// FROM https://forum.qt.io/topic/7328/how-can-i-get-the-default-gateway-ip-using-qt/13
#ifdef Q_OS_WIN
#include <wbemidl.h>
#include <comdef.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Wbemuuid.lib")
#else
#define BUFSIZE 8192
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/socket.h>
#include <linux/rtnetlink.h>
#include <unistd.h>

struct route_info {
	in_addr dstAddr;
	in_addr srcAddr;
	in_addr gateWay;
	char ifName[IF_NAMESIZE];
};

static int readNlSock(int sockFd, char *bufPtr, unsigned int seqNum, int pId)
{
	struct nlmsghdr *nlHdr;
	int readLen = 0, msgLen = 0;
	do {
		if ((readLen = recv(sockFd, bufPtr, BUFSIZE - msgLen, 0)) < 0) {
			::perror("SOCK READ: ");
			return -1;
		}
		nlHdr = (struct nlmsghdr *)bufPtr;
		if ((NLMSG_OK(nlHdr, readLen) == 0) || (nlHdr->nlmsg_type == NLMSG_ERROR)) {
			::perror("Error in received packet");
			return -1;
		}
		if (nlHdr->nlmsg_type == NLMSG_DONE) {
			break;
		}

		bufPtr += readLen;
		msgLen += readLen;

		if ((nlHdr->nlmsg_flags & NLM_F_MULTI) == 0) {
			break;
		}

	} while ((nlHdr->nlmsg_seq != seqNum) || (nlHdr->nlmsg_pid != (unsigned)pId));
	return msgLen;
}

static void parseRoutes(struct nlmsghdr *nlHdr, struct route_info *rtInfo)
{
	struct rtmsg *rtMsg = (struct rtmsg *)NLMSG_DATA(nlHdr);

	if ((rtMsg->rtm_family != AF_INET) || (rtMsg->rtm_table != RT_TABLE_MAIN)) {
		return;
	}

	struct rtattr *rtAttr = (struct rtattr *)RTM_RTA(rtMsg);
	int rtLen = RTM_PAYLOAD(nlHdr);
	for (; RTA_OK(rtAttr, rtLen); rtAttr = RTA_NEXT(rtAttr, rtLen)) {
		switch (rtAttr->rta_type) {
		case RTA_OIF:
			if_indextoname(*(int *)RTA_DATA(rtAttr), rtInfo->ifName);
			break;
		case RTA_GATEWAY:
			rtInfo->gateWay = *(in_addr *)RTA_DATA(rtAttr);
			break;
		case RTA_PREFSRC:
			rtInfo->srcAddr = *(in_addr *)RTA_DATA(rtAttr);
			break;
		case RTA_DST:
			rtInfo->dstAddr = *(in_addr *)RTA_DATA(rtAttr);
			break;
		}
	}
}
#endif


QHostAddress defaultGatewayAddress()
{
	QString ret;
#ifdef Q_OS_WIN
	CoInitialize(NULL);
	if (CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_PKT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, 0) != S_OK) {
		return QHostAddress(ret);
	}

	IWbemLocator *pLoc = NULL;
	if (CoCreateInstance(CLSID_WbemAdministrativeLocator, NULL, CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER, IID_IUnknown, (void **)&pLoc) != S_OK) {
		return QHostAddress(ret);
	}

	IWbemServices *pSvc = NULL;
	if (pLoc->ConnectServer(L"root\\cimv2", NULL, NULL, NULL, 0, NULL, NULL, &pSvc) != S_OK) {
		return QHostAddress(ret);
	}

	IEnumWbemClassObject *pEnumerator = NULL;
	HRESULT hr = pSvc->ExecQuery(L"WQL", L"SELECT * FROM Win32_NetworkAdapterConfiguration WHERE IPEnabled = 'TRUE'", WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);
	if (FAILED(hr)) {
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return QHostAddress(ret);
	}

	IWbemClassObject *pclsObj = NULL;
	while (pEnumerator && ret.isEmpty()) {
		_variant_t vaDefaultIPGateway;
		ULONG uReturn;
		hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
		if (!uReturn) {
			break;
		}

		hr = pclsObj->Get(L"DefaultIPGateway", 0, &vaDefaultIPGateway, NULL, NULL);
		if (hr == WBEM_S_NO_ERROR && vaDefaultIPGateway.vt != VT_NULL) {
			LONG lLow = 0;
			LONG lUp = 0;
			::SafeArrayGetLBound(vaDefaultIPGateway.parray, 1, &lLow);
			::SafeArrayGetUBound(vaDefaultIPGateway.parray, 1, &lUp);
			for (LONG i = lLow; i <= lUp; i++) {
				BSTR bsDefaultIPGateway;
				if (::SafeArrayGetElement(vaDefaultIPGateway.parray, &i, &bsDefaultIPGateway) == S_OK) {
					ret = QString::fromUtf16(reinterpret_cast<const ushort *>(bsDefaultIPGateway));
					::SysFreeString(bsDefaultIPGateway);
					break;
				}
			}
		}
		VariantClear(&vaDefaultIPGateway);
	}

	if (pclsObj) {
		pclsObj->Release();
	}
	if (pEnumerator) {
		pEnumerator->Release();
	}
	if (pSvc) {
		pSvc->Release();
	}
	if (pLoc) {
		pLoc->Release();
	}
	CoUninitialize();
#else
	int sock, msgSeq = 0;
	if ((sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE)) < 0) {
		::perror("Socket Creation: ");
		return QHostAddress(ret);
	}

	char msgBuf[BUFSIZE];
	memset(msgBuf, 0, BUFSIZE);
	struct nlmsghdr *nlMsg = (struct nlmsghdr *)msgBuf;

	nlMsg->nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));
	nlMsg->nlmsg_type = RTM_GETROUTE;

	nlMsg->nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST;
	nlMsg->nlmsg_seq = msgSeq++;
	nlMsg->nlmsg_pid = getpid();

	if (send(sock, nlMsg, nlMsg->nlmsg_len, 0) < 0) {
		return QHostAddress(ret);
	}

	int len;
	if ((len = readNlSock(sock, msgBuf, msgSeq, getpid())) < 0) {
		return QHostAddress(ret);
	}

	struct route_info *rtInfo = (struct route_info *)malloc(sizeof(struct route_info));

	for (; NLMSG_OK(nlMsg, len); nlMsg = NLMSG_NEXT(nlMsg, len)) {
		memset(rtInfo, 0, sizeof(struct route_info));
		parseRoutes(nlMsg, rtInfo);

		if (strstr((char *)inet_ntoa(rtInfo->dstAddr), "0.0.0.0") && !strstr((char *)inet_ntoa(rtInfo->gateWay), "0.0.0.0")) {
			char buf[64];
			inet_ntop(AF_INET, &rtInfo->gateWay, buf, sizeof(buf));
			ret = QString(buf);
			break;
		}
	}

	free(rtInfo);
	close(sock);
#endif
	return QHostAddress(ret);
}



}
