#ifndef BIRTHCONTROL_HPP
#define BIRTHCONTROL_HPP

#include "name/AgentNameGenerator.hpp"
#include "name/GenderGenerator.hpp"
#include "uptime/ConfigureHelper.hpp"
#include "uptime/SharedPointerWrapper.hpp"

#include <QObject>
#include <QTimer>


class Key;
class Associate;

struct DeliveryStatus{
	QStringList messages;
	bool hasLocalKey;
	bool hasNodeIdentity{false};
	bool hasLocalKeyID{false};
	bool hasNodeIdentityID{false};
	bool hasNodeIdentityIDMatchesLocalKeyID{false};
	bool hasLocalKeyPublic{false};
	bool hasLocalKeyPrivate{false};
	bool hasNodeIdentityName{false};
	bool hasNodeIdentityGender{false};
	bool hasNodeIdentityType{false};
	bool hasNodeIdentityRole{false};
	
	QSharedPointer<Key> localKey;
	QSharedPointer<Associate> nodeIdentity;
	QString toString(){
		QString out;
		if(!messages.isEmpty()){
			out += messages.join("\n, ");
		}
		return out;
	}
	bool isOK(){
		return hasLocalKey
			&& hasNodeIdentity
			&& hasNodeIdentity
			&& hasLocalKeyID
			&& hasNodeIdentityID
			&& hasNodeIdentityIDMatchesLocalKeyID
			&& hasLocalKeyPublic
			&& hasLocalKeyPrivate
			&& hasNodeIdentityName
			&& hasNodeIdentityGender
			&& hasNodeIdentityType
			&& hasNodeIdentityRole;
	}
};


class Node;
class BirthControl: public QObject
{
	Q_OBJECT
private:
	QSharedPointer<Node> mNode;
	ConfigureHelper mConfigureHelper;
	AgentNameGenerator mAgentNameGenerator;
	GenderGenerator mGenderGenerator;
	QTimer mBirthTimer;
	bool mTimerCompleted{false};
	bool mKeygenCompleted{false};
	QString mDoneMessage;
	bool mDoneStatus{false};
public:
	static const quint64 MINIMUM_BIRTH_TIME;

public:
	explicit BirthControl(QObject *parent = nullptr);
	
	
private:
	void configureTimer();
	void onBirthComplete(bool success, const QString &message);


public:
	void configure(QSharedPointer <Node> node);
	// Start delivery
	void birth(const QString &name = QString(), const QString &gender = QString());
	// Start un-delivery
	void unBirth();
	// Helper to debug init process. Prints out the current status of initialization
	DeliveryStatus statusAssesment();
	
signals:
	void birthProgress(const QString &step, int index, int total);
	void birthComplete(bool success, const QString &message);
	
	
};

#endif // BIRTHCONTROL_HPP
