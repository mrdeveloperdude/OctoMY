#include "utility/string/String.hpp"

#include "uptime/New.hpp"
#include "uptime/MethodGate.hpp"

#include <QCoreApplication>

#include <QThread>

#include <cmath>
//#include <cfloat>


namespace utility
{


namespace string
{



QString humanReadableElapsedSeconds(long double seconds,int prec,bool prep,long double resolution)
{
	struct stance {
		long double interval;
		QString name;
		quint8 pad[8]= {0}; // Get rid of alignment padding warning
	};
	const stance stances[] {
		{0.0l,"time"}
		,{0.000001l,"ns"}
		,{0.001l,"ms"}
		,{1,"sec"}
		,{60,"min"}
		,{60*60,"hr"}
		,{60*60*24,"days"}
		,{60*60*24*7,"weeks"}
		,{60*60*24*30,"months"}
		,{60*60*24*365,"years"}
		,{60*60*24*365*10,"decades"}
		//		,{60*60*24*365*100,"centuries"}
		//		,{60*60*24*365*1000,"millenia"}
	};
	int i=1,l=sizeof(stances)/sizeof(stance)-1;
	bool past=true;
	if(seconds<0.0l) {
		seconds=-seconds;
		past=false;
	}
	if(std::isnan(seconds) ) {
		seconds=0.0l;
	}
	if(0.0l==seconds) {
		seconds=0.0l;
		i=0;
	} else {
		const long double lim=qMax(seconds,resolution);
		while (i<l && lim > stances[i].interval) {
			i++;
		}
	}
	i=qMax(0,i-1);
	QString ret= (prep?(past?"":"in "):"");
	ret+=QString::number(static_cast<double>(seconds / stances[i].interval), 'f', prec);
	ret+=" ";
	ret+=stances[i].name;
	ret+=(prep?(past?" ago":""):"");
	return ret;
}

QString formattedDateFromSeconds(long double seconds)
{
	qint64 ms=static_cast<qint64>(seconds*1000.0l);
	return formattedDateFromMS(ms);
}

QString formattedDateFromMS(qint64 ms)
{
	return QDateTime::fromMSecsSinceEpoch(ms).toString("yyyy-MM-dd hh:mm:ss.zzz t") ;
}

QString formattedDateFromMS(quint64 ms)
{
	return formattedDateFromMS(static_cast<qint64>(ms));
}

QString humanReadableSize(qint64 bytes,int prec)
{
	struct stance {
		qint64 interval;
		QString name;
	};
	const qint64 K=1024;
	const stance stances[] {
		{1,"bytes"}
		,{K,"KiB"}
		,{K*K,"MiB"}
		,{K*K*K,"GiB"}
		,{K*K*K*K,"TiB"}
		,{K*K*K*K*K,"PiB"}
		,{K*K*K*K*K*K,"EiB"}
//        ,{K*K*K*K*K*K*K,"ZiB"}
//        ,{K*K*K*K*K*K*K*K,"YiB"}
	};
	int i=1,l=sizeof(stances)/sizeof(stance)-1;
	if(bytes<0) {
		bytes=-bytes;
	}
	if(0==bytes) {
		bytes=0;
		i=0;
	} else {
		while (i<l && bytes > stances[i].interval) {
			i++;
		}
	}
	i=qMax(0,i-1);
	const long double bytesf=bytes;
	const long double intervalf=stances[i].interval;
	QString ret=QString::number(static_cast<double>(bytesf/ intervalf), 'f', prec);
	ret+=" ";
	ret+=stances[i].name;
	return ret;
}







size_t snprint_hex(char *dst, size_t size, const uint8_t *pbtData, const size_t szBytes)
{
	size_t  szPos;
	size_t res = 0;
	for (szPos = 0; szPos < szBytes; szPos++) {
		res += static_cast<size_t>(snprintf(dst + res, size - res, "%02X", pbtData[szPos]));
	}
	res += sizeof('\0');//snprintf(dst + res, size - res, "\0");
	return res;
}


QString toHEX(const uint8_t *pbtData, const size_t szBytes)
{
	QString out;
	size_t  szPos;
	//		size_t res = 0;
	uint8_t dst[20];
	for (szPos = 0; szPos < szBytes; szPos++) {
		snprintf(reinterpret_cast<char *>(dst), 0, "%02X", pbtData[szPos]);
		out+=QString::fromLatin1(reinterpret_cast<char *>(dst), 2);
	}
	return out;
}



QString  hexdump(void *mem, unsigned int len, QString topic)
{
	static const int HEXDUMP_COLS=16;
	QStringList sl;
	if(!topic.trimmed().isEmpty()) {
		sl<<"----- "<<topic<<" -----\n";
	}
	for(unsigned int i = 0; i < len + ((len % HEXDUMP_COLS) ? (HEXDUMP_COLS - len % HEXDUMP_COLS) : 0); i++) {
		//print offset
		if(i % HEXDUMP_COLS == 0) {
			sl<<QString("0x%1: ").arg(i, 4, 16, static_cast<QChar>(QLatin1Char('0')));
		}
		//print hex data
		if(i < len) {
			sl<<QString("%1").arg(static_cast<int>(0xFF & (static_cast<char*>(mem))[i]), 2, 16, static_cast<QChar>(QLatin1Char('0')));
		} else {
			//end of block, just aligning for ASCII dump
			sl<<"   ";
		}
		//print ASCII dump
		if(i % HEXDUMP_COLS == (HEXDUMP_COLS - 1)) {
			sl<<"  ";
			for(unsigned int j = i - (HEXDUMP_COLS - 1); j <= i; j++) {
				if(j >= len) {
					//end of block, not really printing
					sl<<" ";
				} else if(isprint(((static_cast<char*>(mem))[j] & 0x7F))) {
					//printable char
					sl<<QString(static_cast<QChar>(0xFF & (static_cast<char*>(mem))[j]));
				} else {
					//other char
					sl<<".";
				}
			}
			sl<<"\n";
		}
	}
	return sl.join("");
}


bool isPositiveInteger(QString s)
{
	QString p=s.replace(QRegExp("[^0-9]"),"");
	return p.length()==s.length();
}


QString perror()
{
	return QString::fromLatin1(strerror(errno));
}


qint64 parseDate(QString str, QString fmt)  //2013-08-14T20:26:01.7526+02:00
{
	QDateTime d = QDateTime::fromString(str,fmt);
	qint64 ts= d.toMSecsSinceEpoch();
	if(-210866803200000==ts) {
		ts=0;
	}
	//qDebug()<<"parseDate("<<str<<")="<<ts;
	return ts;
}


QString toHash(QString input, QCryptographicHash::Algorithm algo, bool upperCase)
{
	OC_FUNCTIONGATE();
	QCryptographicHash ch(algo);
	ch.addData(input.toUtf8());
	QByteArray raw=ch.result();
	QString out=raw.toHex();
	if(upperCase) {
		out=out.toUpper();
	} else {
		out=out.toLower();
	}
	return out;
}


QString nameToNormalForm(QString name)
{
	OC_FUNCTIONGATE();
	if(name.isEmpty()) {
		return name;
	}
	name = name.toLower();
	name[0] = name[0].toUpper();
	for (int i = 1; i < name.length(); i++) {
		if(!name.at(i).isLetter() && name.at(i+1).isLetter()) {
			name[i+1] = name[i+1].toUpper();
		}
	}
	return name;
}


QString networkErrorToString( QNetworkReply::NetworkError ne)
{
	OC_FUNCTIONGATE();
#define NETWORK_ERROR_TO_STRING_STANSA(a)  case(QNetworkReply::a): return #a + QStringLiteral("(") +QString::number(QNetworkReply::a) + QStringLiteral(")")
	switch(ne) {
		NETWORK_ERROR_TO_STRING_STANSA(NoError);
		NETWORK_ERROR_TO_STRING_STANSA(ConnectionRefusedError );
		NETWORK_ERROR_TO_STRING_STANSA(RemoteHostClosedError);
		NETWORK_ERROR_TO_STRING_STANSA(HostNotFoundError);
		NETWORK_ERROR_TO_STRING_STANSA(TimeoutError);
		NETWORK_ERROR_TO_STRING_STANSA(OperationCanceledError);
		NETWORK_ERROR_TO_STRING_STANSA(SslHandshakeFailedError);
		NETWORK_ERROR_TO_STRING_STANSA(TemporaryNetworkFailureError);
		NETWORK_ERROR_TO_STRING_STANSA(NetworkSessionFailedError);
		NETWORK_ERROR_TO_STRING_STANSA(BackgroundRequestNotAllowedError);
		NETWORK_ERROR_TO_STRING_STANSA(InsecureRedirectError);
		NETWORK_ERROR_TO_STRING_STANSA(TooManyRedirectsError);
		NETWORK_ERROR_TO_STRING_STANSA(UnknownNetworkError );
		NETWORK_ERROR_TO_STRING_STANSA(ProxyConnectionRefusedError );
		NETWORK_ERROR_TO_STRING_STANSA(ProxyConnectionClosedError);
		NETWORK_ERROR_TO_STRING_STANSA(ProxyNotFoundError);
		NETWORK_ERROR_TO_STRING_STANSA(ProxyTimeoutError);
		NETWORK_ERROR_TO_STRING_STANSA(ProxyAuthenticationRequiredError);
		NETWORK_ERROR_TO_STRING_STANSA(UnknownProxyError );
		NETWORK_ERROR_TO_STRING_STANSA(ContentAccessDenied );
		NETWORK_ERROR_TO_STRING_STANSA(ContentOperationNotPermittedError);
		NETWORK_ERROR_TO_STRING_STANSA(ContentNotFoundError);
		NETWORK_ERROR_TO_STRING_STANSA(AuthenticationRequiredError);
		NETWORK_ERROR_TO_STRING_STANSA(ContentReSendError);
		NETWORK_ERROR_TO_STRING_STANSA(ContentConflictError);
		NETWORK_ERROR_TO_STRING_STANSA(ContentGoneError);
		NETWORK_ERROR_TO_STRING_STANSA(UnknownContentError);
		NETWORK_ERROR_TO_STRING_STANSA(ProtocolUnknownError);
		NETWORK_ERROR_TO_STRING_STANSA(ProtocolInvalidOperationError);
		NETWORK_ERROR_TO_STRING_STANSA(ProtocolFailure);
		NETWORK_ERROR_TO_STRING_STANSA(InternalServerError);
		NETWORK_ERROR_TO_STRING_STANSA(OperationNotImplementedError);
		NETWORK_ERROR_TO_STRING_STANSA(ServiceUnavailableError);
		NETWORK_ERROR_TO_STRING_STANSA(UnknownServerError);
	}
#undef NETWORK_ERROR_TO_STRING_STANSA
	return "UNKNOWN("+QString::number(ne)+")";
}


QString socketErrorToString( QAbstractSocket::SocketError se)
{
	OC_FUNCTIONGATE();
#define SOCKET_ERROR_TO_STRING_STANSA(a)  case(QAbstractSocket::a): return #a + QStringLiteral("(") +QString::number(QAbstractSocket::a) + QStringLiteral(")")
	switch(se) {
		SOCKET_ERROR_TO_STRING_STANSA(ConnectionRefusedError);
		SOCKET_ERROR_TO_STRING_STANSA(RemoteHostClosedError);
		SOCKET_ERROR_TO_STRING_STANSA(HostNotFoundError);
		SOCKET_ERROR_TO_STRING_STANSA(SocketAccessError);
		SOCKET_ERROR_TO_STRING_STANSA(SocketResourceError);
		SOCKET_ERROR_TO_STRING_STANSA(SocketTimeoutError);
		SOCKET_ERROR_TO_STRING_STANSA(DatagramTooLargeError);
		SOCKET_ERROR_TO_STRING_STANSA(NetworkError);
		SOCKET_ERROR_TO_STRING_STANSA(AddressInUseError);
		SOCKET_ERROR_TO_STRING_STANSA(SocketAddressNotAvailableError);
		SOCKET_ERROR_TO_STRING_STANSA(UnsupportedSocketOperationError);
		SOCKET_ERROR_TO_STRING_STANSA(UnfinishedSocketOperationError);
		SOCKET_ERROR_TO_STRING_STANSA(ProxyAuthenticationRequiredError);
		SOCKET_ERROR_TO_STRING_STANSA(SslHandshakeFailedError);
		SOCKET_ERROR_TO_STRING_STANSA(ProxyConnectionRefusedError);
		SOCKET_ERROR_TO_STRING_STANSA(ProxyConnectionClosedError);
		SOCKET_ERROR_TO_STRING_STANSA(ProxyConnectionTimeoutError);
		SOCKET_ERROR_TO_STRING_STANSA(ProxyNotFoundError);
		SOCKET_ERROR_TO_STRING_STANSA(ProxyProtocolError);
		SOCKET_ERROR_TO_STRING_STANSA(OperationError);
		SOCKET_ERROR_TO_STRING_STANSA(SslInternalError);
		SOCKET_ERROR_TO_STRING_STANSA(SslInvalidUserDataError);
		SOCKET_ERROR_TO_STRING_STANSA(TemporaryError);
		SOCKET_ERROR_TO_STRING_STANSA(UnknownSocketError);
	}
#undef SOCKET_ERROR_TO_STRING_STANSA
	return "UNKNOWN("+QString::number(se)+")";
}


QString padstring(int level, QString base)
{
	QString out="";
	if(""==base) {
		base=" ";
	}
	const int l=base.length();
	for(int i=0; i<level; ++i) {
		out+=QString(base.at(i%l));
	}
	return out;
}


QString toSoundex(QString in)
{
	QString out="";
	const int l=in.size();
	if(l<1) {
		return "0000";
	}
	in=in.toLower();
	out+=in[0];
	QChar lastChar='0';
	int ct=0;
	for(int i=1; i<l; ++i) {
		QCharRef ch=in[i];
		if(lastChar!=ch) {
			switch(ch.toLatin1()) {
			case('b'):
			case('f'):
			case('p'):
			case('v'): {
				out+='1';
				ct++;
			}
			break;
			case('c'):
			case('g'):
			case('j'):
			case('k'):
			case('q'):
			case('s'):
			case('x'):
			case('z'): {
				out+='2';
				ct++;
			}
			break;
			case('d'):
			case('t'): {
				out+='3';
				ct++;
			}
			break;
			case('l'): {
				out+='4';
				ct++;
			}
			break;
			case('m'):
			case('n'): {
				out+='4';
				ct++;
			}
			break;
			case('r'): {
				out+='5';
				ct++;
			}
			break;
			}
			if(ct==4) {
				break;
			}
			lastChar=ch;
		}
	}
	if(out.size()<4) {
		out+=QString("0").repeated(4-out.size());
	}
	return out;
}


HandleCounter::HandleCounter()
	: handleCounter(0)
	, handleMap()
	, handleMutex(QMutex::Recursive)
{

}


QString HandleCounter::handleCounterString(Qt::HANDLE h)
{
	if(nullptr==h) {
		return "H-null";
	}
	QMutexLocker lock(&handleMutex);
	if(!handleMap.contains(h)) {
		handleCounter++;
		handleMap.insert(h,handleCounter);
	}
	return "H-"+QString::number(handleMap[h]);
}


static HandleCounter hc;


int levenshtein_distance(const QString &s1, const QString  &s2)
{
	int s1len = s1.size();
	int s2len = s2.size();

	auto column_start = static_cast<decltype(s1len)>(1);

	auto column = OC_NEW decltype(s1len)[static_cast<unsigned long>(s1len + 1)];
	std::iota(column + column_start, column + s1len + 1, column_start);

	for (auto x = column_start; x <= s2len; x++) {
		column[0] = x;
		auto last_diagonal = x - column_start;
		for (auto y = column_start; y <= s1len; y++) {
			auto old_diagonal = column[y];
			auto possibilities = {
				column[y] + 1,
				column[y - 1] + 1,
				last_diagonal + (s1[y - 1] == s2[x - 1]? 0 : 1)
			};
			column[y] = std::min(possibilities);
			last_diagonal = old_diagonal;
		}
	}
	auto result = column[s1len];
	delete[] column;
	column = nullptr;
	return result;
}


QString currentThreadID()
{
	return hc.handleCounterString(QThread::currentThreadId());
}

}
}
