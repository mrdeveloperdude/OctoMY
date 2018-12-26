#include "utility/string/String.hpp"

#include <QCoreApplication>

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
		quint8 pad[8]={0}; // Get rid of alignment padding warning
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

}
}
