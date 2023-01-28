#include "Network.hpp"

#include <QHostAddress>
// FROM https://forum.qt.io/topic/7328/how-can-i-get-the-default-gateway-ip-using-qt/13
#if defined(Q_OS_WIN) && defined(_MSC_VER)
//#include <wbemidl.h>
#include <wbemcli.h>
#include <comdef.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Wbemuuid.lib")
#elif defined(Q_OS_LINUX)
#define BUFSIZE 8192
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/socket.h>
#include <linux/rtnetlink.h>
#include <unistd.h>
#include <cstring>

#endif


namespace utility
{

namespace network
{


#if defined(Q_OS_LINUX)

struct route_info {
	in_addr dstAddr;
	in_addr srcAddr;
	in_addr gateWay;
	char ifName[IF_NAMESIZE];
};

static int readNlSock(int sockFd, char *bufPtr, unsigned int seqNum, int pId)
{
	struct nlmsghdr *nlHdr;
	ssize_t readLen = 0;
	size_t msgLen = 0;
	do {
		if ((readLen = recv(sockFd, bufPtr, BUFSIZE - msgLen, 0)) < 0) {
			qWarning()<<"SOCK READ: "<<QString(::strerror(errno));
			//::perror("SOCK READ: ");
			return -1;
		}
		nlHdr = reinterpret_cast<struct nlmsghdr *>(bufPtr);
		if ((NLMSG_OK(nlHdr, readLen) == 0) || (nlHdr->nlmsg_type == NLMSG_ERROR)) {
			qWarning()<<"Error in received packet: "<<QString(::strerror(errno));
			//::perror("Error in received packet");
			return -1;
		}
		if (nlHdr->nlmsg_type == NLMSG_DONE) {
			break;
		}

		bufPtr += readLen;
		msgLen += static_cast<size_t>(readLen);

		if ((nlHdr->nlmsg_flags & NLM_F_MULTI) == 0) {
			break;
		}

	} while ((nlHdr->nlmsg_seq != seqNum) || (nlHdr->nlmsg_pid != static_cast<unsigned int>(pId)));
	return static_cast<int>(msgLen);
}

static void parseRoutes(struct nlmsghdr *nlHdr, struct route_info *rtInfo)
{
	struct rtmsg *rtMsg = static_cast<struct rtmsg *>(NLMSG_DATA(nlHdr));

	if ((rtMsg->rtm_family != AF_INET) || (rtMsg->rtm_table != RT_TABLE_MAIN)) {
		return;
	}

	auto tmp=RTM_RTA(rtMsg);
	struct rtattr *rtAttr = reinterpret_cast<struct rtattr *>(tmp);
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
#if defined(Q_OS_WIN) && defined(_MSC_VER)
	CoInitialize(NULL);
	if (CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_PKT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, 0) != S_OK) {
		return QHostAddress(ret);
	}

	IWbemLocator *pLoc = NULL;
	if (CoCreateInstance(CLSID_WbemLocator, NULL, CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER, IID_IUnknown, (void **)&pLoc) != S_OK) {
		return QHostAddress(ret);
	}

	IWbemServices *pSvc = NULL;
	if (pLoc->ConnectServer(BSTR (L"root\\cimv2"), NULL, NULL, NULL, 0, NULL, NULL, &pSvc) != S_OK) {
		return QHostAddress(ret);
	}

	IEnumWbemClassObject *pEnumerator = NULL;
	HRESULT hr = pSvc->ExecQuery(BSTR (L"WQL"), BSTR ( L"SELECT * FROM Win32_NetworkAdapterConfiguration WHERE IPEnabled = 'TRUE'"), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);
	//HRESULT hr = pSvc->ExecQuery(L"WQL", L"SELECT * FROM Win32_NetworkAdapterConfiguration WHERE IPEnabled = 'TRUE'", WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);
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
			SafeArrayGetLBound(vaDefaultIPGateway.parray, 1, &lLow);
			SafeArrayGetUBound(vaDefaultIPGateway.parray, 1, &lUp);
			for (LONG i = lLow; i <= lUp; i++) {
				BSTR bsDefaultIPGateway;
				if (SafeArrayGetElement(vaDefaultIPGateway.parray, &i, &bsDefaultIPGateway) == S_OK) {
					ret = QString::fromUtf16(reinterpret_cast<const ushort *>(bsDefaultIPGateway));
					SysFreeString(bsDefaultIPGateway);
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

#elif defined(Q_OS_LINUX)
	int sock, msgSeq = 0;
	if ((sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE)) < 0) {
		qWarning()<<"Socket Creation: "<<QString(::strerror(errno));
		//::perror("Socket Creation: ");
		return QHostAddress(ret);
	}

	char msgBuf[BUFSIZE];
	memset(msgBuf, 0, BUFSIZE);
	struct nlmsghdr *nlMsg = reinterpret_cast<struct nlmsghdr *>(msgBuf);

	nlMsg->nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));
	nlMsg->nlmsg_type = RTM_GETROUTE;

	nlMsg->nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST;
	nlMsg->nlmsg_seq = static_cast<__u32>(msgSeq);
	msgSeq++;
	nlMsg->nlmsg_pid = static_cast<__u32>(getpid());

	if (send(sock, nlMsg, nlMsg->nlmsg_len, 0) < 0) {
		return QHostAddress(ret);
	}

	int len;
	if ((len = readNlSock(sock, msgBuf, static_cast<unsigned int>(msgSeq), getpid())) < 0) {
		return QHostAddress(ret);
	}

	struct route_info *rtInfo = static_cast<struct route_info *>(malloc(sizeof(struct route_info)));

	for (; NLMSG_OK(nlMsg, len); nlMsg = NLMSG_NEXT(nlMsg, len)) {
		memset(rtInfo, 0, sizeof(struct route_info));
		parseRoutes(nlMsg, rtInfo);

		if (strstr(static_cast<char *>(inet_ntoa(rtInfo->dstAddr)), "0.0.0.0") && !strstr(static_cast<char *>(inet_ntoa(rtInfo->gateWay)), "0.0.0.0")) {
			char buf[64];
			inet_ntop(AF_INET, &rtInfo->gateWay, buf, sizeof(buf));
			ret = QString(buf);
			break;
		}
	}

	free(rtInfo);
	close(sock);
#else
	qWarning()<<"ERROR: Platform does not support getting default gateway";
#endif
	return QHostAddress(ret);
}



}
}
