#ifndef PUPPET_HPP
#define PUPPET_HPP

#include "utility/Utility.hpp"
#include <QVector>
#include <QDebug>


/**
 * A Puppet represents the physical makeup of the robot.
 * It is constructed from Members arranged in a tree.
 *
 * Members represent a moveable part of the robot with an actuator.
 * The top level Member is called the "root" and has a special role.
 * Members may also have Equipmnt attached to them.
 *
 * Equipment represents all kinds of sensors and gadgets that are not part of
 * the robot's locomotion system. Examples are GPS, Gyro, Camera, LEDs,
 * solar panel etc.
 *
 * A Pose represents the current actuator positions for a puppet.
 * Poses can be strung together as keyframes to form an animation sequence.
 */

template <typename T>
class ActuatorDescriptor{
		T range; //radians from min to max
		T resolution; //lowest angle of movement in radians
		T lowTrim;//At which angle does range start
		T highTrim;//At which angle does range stop
		T axisAlign;//At which angle is the actuator aligned
};


template <typename T>
class MemberDescriptor{
		T length; //Length of this member in mm
};

template <typename T>
class Member;

template <typename T>
class Puppet;

template <typename T>
class Equipment{
	private:
		Member<T> *m_member;
	public:
		explicit Equipment(Member<T> &member)
			: m_member(&member)
		{
			m_member->addEquipment(*this);
		}

		virtual ~Equipment(){

		}

		QString toString(int level=0){
			QString out;
			out+=utility::padstring(level);
			out+="Equipment X";
			return out;
		}
};

template <typename T>
class Member{
	private:
		quint32 id;
		static quint32 sid;
		Member<T> *m_parent;
		Puppet<T> *m_puppet;
		ActuatorDescriptor<T> m_actuator;
		MemberDescriptor<T> m_memberDescriptor;
		QVector<Member<T> *> m_children;
		QVector<Equipment<T> *> m_equipment;
		QString m_name;
	public:
		explicit Member(Puppet<T> &puppet, Member<T> *parent, QString name="")
			: id(sid++)
			, m_parent(parent)
			, m_puppet(&puppet)
			, m_name(""==name?"Unnamed "+QString::number(id)+"/"+QString::number(sid):name)
		{
			m_puppet->addMember(*this);
		}

		virtual ~Member(){

		}

		Member<T> *parent(){
			return m_parent;
		}
		Puppet<T> &puppet(){
			return *m_puppet;
		}

		void addMember(Member<T> &member){
			m_children.push_back(*member);
		}


		void addEquipment(Equipment<T> &equipment){
			if(*this!=equipment.member()){
				qDebug()<<"ERROR: Specified equipment was not previously associated with member when adding";
				return;
			}
			m_equipment.push_back(equipment);
		}

		QString toString(bool includeEquipment=true, int level=0){
			QString out;
			out+=utility::padstring(level);
			out+="Member '"+m_name+"'\n";
			level++;
			if(includeEquipment){
				out+=utility::padstring(level);
				out+="Equipemnt:\n";
				for(auto it=m_equipment.begin(), e=m_equipment.end(); e!=it ; ++it){
					out+=*it->toString(level);
				}
			}
			out+=utility::padstring(level);
			out+="Children:\n";
			for(auto it=m_children.begin(), e=m_children.end(); e!=it ; ++it){
				out+=*it->toString(includeEquipment, level);
			}

			level--;
			return out;
		}
};

template <typename T>
quint32 Member<T>::sid=0;


template <typename T>
class Puppet{
	private:
		Member<T> *m_root;
		QVector<Member<T> *> m_members;

	public:
		explicit Puppet()
			: m_root(0)
		{
		}

		virtual ~Puppet(){
		}

		void addMember(Member<T> &member){
			if(*this!=member.puppet()){
				qDebug()<<"ERROR: Specified member was not previously associated with puppet when setting as root";
				return;
			}
			m_members.push_back(member);
			if(0==member.parent()){
				m_root=&member;
			}
		}

		QString toString(bool includeEquipment=true){
			if(0==m_root){
				return "Empty Puppet";
			}
			return m_root->toString(includeEquipment,0);
		}

};


#endif // PUPPET_HPP
