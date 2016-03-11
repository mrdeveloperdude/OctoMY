#ifndef LIMB_HPP
#define LIMB_HPP


#include <QVector>
#include <QVector3D>
#include <QSet>

#include <qopengl.h>

#include "gear/gear.h"

class Simulation;


enum HingeType{
	HINGE_STIFF //0-dof (noop)
	, HINGE_REVOLUTE //1-dof
	, HINGE_UNIVERSAL //2-dof
	, HINGE_BALL //3-dof
};



////////////////////////////////////////////////

class Hinge{
	private:
		HingeType type;
		QVector3D axis1;
		QVector3D axis2;
	public:
		explicit Hinge(
				HingeType type=HINGE_REVOLUTE
				, QVector3D axis1=QVector3D (1,0,0)
				, QVector3D axis2=QVector3D (1,0,0)
		);
};



////////////////////////////////////////////////////////////////////////

class Limb{
	private:
		Simulation &sim;
		Limb *parent;
		QVector3D pos;
		QVector3D dir;
		QVector3D up;
		qreal length;
		Hinge hinge;
		QSet<Limb *> children;
		GBody *link;
		GJointRevolute *joint;


	public:
		explicit Limb(Simulation &sim
					  , Limb *parent=0
				, QVector3D pos=QVector3D (0,0,0)
				, QVector3D dir=QVector3D (1,0,0)
				, QVector3D up=QVector3D (0,1,0)
				, qreal length=1
				, HingeType hingeType=HINGE_STIFF
				, QVector3D hingeAxis1=QVector3D (0,0,1)
				, QVector3D hingeAxis2=QVector3D (0,1,0)
				);
		virtual ~Limb();
	public:

		void addChild(Limb *child);
		void update(float d, float dt);
		void paint();


};


GSystem *createGEARSystem();

#endif // LIMB_HPP
