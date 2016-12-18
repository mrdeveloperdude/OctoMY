#ifndef HARDWARETEMPLATE_HPP
#define HARDWARETEMPLATE_HPP


#include "puppet/PoseMapping.hpp"

class HardwareTemplate
{
private:
	static QVector<HardwareTemplate *> mTemplates;

public:

	static const QString PROTOCOL_SERVOTOR32_2_1;
	static const QString INTERFACE_SERVOTOR32;
private:
	const QString mName;
	const QString mDescription;
	const QString mIconURL;
	const QString mInterface;
	const QString mProtocol;
	const PoseMapping mPoseMapping;

public:

	explicit HardwareTemplate(  const QString &name, const QString &description, const QString &iconURL,const QString &interface, const QString &protocol, const PoseMapping &poseMapping);

public:

	QString name() const;
	QString description() const;

	QString iconURL() const;
	QString interface()const;
	QString protocol()const;
	quint32 poseSize()const;

	const PoseMapping poseMapping() const;

public:



	static QVector<HardwareTemplate *> templates();



};


#endif // HARDWARETEMPLATE_HPP
