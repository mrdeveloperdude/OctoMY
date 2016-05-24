#ifndef PUNCHREGISTRY_HPP
#define PUNCHREGISTRY_HPP


#include <QDateTime>
#include <QMap>
#include <QVariantMap>


struct PunchPartner{

		QString public_address;
		quint16 public_port;
		QString local_address;
		quint16 local_port;
		quint64 lastTime;

		PunchPartner()
			: public_port(0)
			, local_port(0)
			, lastTime (0)
		{

		}

		PunchPartner(QString public_address, quint16 public_port, QString local_address, quint16 local_port)
			: public_address(public_address)
			, public_port(public_port)
			, local_address(local_address)
			, local_port(local_port)
			, lastTime (QDateTime::currentMSecsSinceEpoch())
		{

		}

		PunchPartner(const PunchPartner &o)
			: public_address(o.public_address)
			, public_port(o.public_port)
			, local_address(o.local_address)
			, local_port(o.local_port)
			, lastTime (o.lastTime)
		{

		}


		bool operator==(const PunchPartner &o) const {
			return o.public_address == public_address
					&& o.public_port == public_port
					&& o.local_address == local_address
					&& o.local_port == local_port;
			//Disregard lastTime on purpose
		}

		bool isSet(){
			return (0!=local_port);
		}

		QVariantMap toVariantMap(){
			QVariantMap map;
			map["public_address"]=public_address;
			map["public_port"]=public_port;
			map["local_address"]=local_address;
			map["local_port"]=local_port;
			map["lastTime"]=lastTime;
			return map;
		}

};



struct UDPPunch{

		PunchPartner a,b;
		QString token;

		UDPPunch(QString token)
			: token(token)
		{

		}

		bool set(PunchPartner p){
			if(!a.isSet() || a==p){
				a=p;
				return true;
			}
			if(!b.isSet() || b==p){
				b=p;
				return true;
			}
			return false;
		}

		PunchPartner theOther(PunchPartner p){
			if(p==a){
				return b;
			}
			return a;
		}

		QVariantMap toVariantMap(){
			QVariantMap map;
			map["token"]=QVariant(token);
			map["a"]=a.toVariantMap();
			map["b"]=b.toVariantMap();
			return map;
		}

};



class PunchRegistry: public QMap<QString, UDPPunch *>{
	public:
		bool update(QString punchToken, QString public_address, quint16 public_port, QString local_address, quint16 local_port){
			if(contains(punchToken)){
				UDPPunch *punch=new UDPPunch(punchToken);
				if(nullptr!=punch){
					punch->set(PunchPartner(public_address, public_port, local_address, local_port));
				}
			}
			else{
				UDPPunch *punch=operator[](punchToken);
				if(nullptr!=punch){
					if(punch->set(PunchPartner(public_address, public_port, local_address, local_port))){
						return true;
					}
				}
			}
			return false;
		}


};

#endif // PUNCHREGISTRY_HPP
