#include "HardwareTemplate.hpp"



QVector<HardwareTemplate *> HardwareTemplate::mTemplates;

const QString HardwareTemplate::PROTOCOL_SERVOTOR32_2_1("PROTOCOL_SERVOTOR_2_1");
const QString HardwareTemplate::INTERFACE_SERVOTOR32("INTERFACE_SERVOTOR32");



HardwareTemplate::HardwareTemplate(  const QString &name, const QString &description, const QString &iconURL,const QString &interface, const QString &protocol, const PoseMapping &poseMappings)
	: mName(name)
	, mDescription(description)
	, mIconURL(iconURL)
	, mInterface(interface)
	, mProtocol(protocol)
	, mPoseMapping(poseMappings)
{

}



QString HardwareTemplate::name() const
{
	return mName;
}
QString HardwareTemplate::description() const
{
	return mDescription;
}

QString HardwareTemplate::iconURL() const
{
	return mIconURL;
}
QString HardwareTemplate::interface() const
{
	return mInterface;
}
QString HardwareTemplate::protocol() const
{
	return mProtocol;
}

quint32 HardwareTemplate::poseSize() const
{
	return mPoseMapping.size();
}

const PoseMapping HardwareTemplate::poseMapping() const
{
	return mPoseMapping;
}






QVector<HardwareTemplate *> HardwareTemplate::templates()
{
	//Only do this once
	if(mTemplates.isEmpty()) {
		//HardwareTemplate(  const QString &name, const QString &description, const QString &iconURL,const QString &interface, const QString &protocol, const PoseMapping &poseMapping);
		{
			PoseMapping pmCar(2);
			pmCar.setName(0,"Throttle");
			pmCar.setName(1,"Steering");

			HardwareTemplate *htCar=new HardwareTemplate("RC Car","General purpose RC Car with throttle and steering as only two inputs", ":/icons/wheels.svg", "/dev/ttyACM0","servotor32 2.1", pmCar);
			mTemplates<<htCar;
		}
		{
			PoseMapping pmHexapod(18);
			int index=0;
			for(int leg=0; leg<6; ++leg) {
				for(int limb=0; limb<3; ++limb) {
					pmHexapod.setName(index, QString("Leg %1-%2").arg(leg).arg(limb));
					index++;
				}
			}
			HardwareTemplate *htHexapod=new HardwareTemplate("Hexapod","General purpose hexapod (legged robot with 6 legs) each leg with 3 servos totalling 18 inputs. Enumerated in leg first, limb second, clockwize, outward starting at leg directly to the right of 'head'.", ":/icons/hexapod.svg", "/dev/ttyACM0","servotor32 2.1", pmHexapod);
			mTemplates<<htHexapod;
		}
	}
	return mTemplates;
}
