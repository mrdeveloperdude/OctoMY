#include "Limb.hpp"

#include <qmath.h>

#include "Simulation.hpp"


Hinge::Hinge(
		HingeType type
		, QVector3D axis1
		, QVector3D axis2

		)
	: type(type)
	, axis1(axis1)
	, axis2(axis2)
{

}

////////////////////////////////////////////////


Limb::Limb(Simulation &sim
		   , Limb *parent
		   , QVector3D pos
		   , QVector3D dir
		   , QVector3D up
		   , qreal length
		   , HingeType hingeType
		   , QVector3D hingeAxis1
		   , QVector3D hingeAxis2
		   )
	: sim(sim)
	, parent(parent)
	, pos(pos)
	, dir(dir)
	, up(up)
	, length(length)
	, hinge(hingeType, hingeAxis1, hingeAxis2)
	, link(new GBody)
	, joint(new GJointRevolute)
{
	if(0!=parent){
		parent->addChild(this);
	}

	double Lx = 0.3, Ly = 0.05, Lz = 0.05; // link dimension
	double M = 1; // link mass
	double Ixx = 1./12.*M*(Ly*Ly+Lz*Lz), Iyy = 1./12.*M*(Lx*Lx+Lz*Lz), Izz = 1./12.*M*(Lx*Lx+Ly*Ly), Ixy = 0, Ixz = 0, Iyz = 0; // moments of inertia with respect to the mass center
	SE3 T_link_com = SE3(); // transformation from {link} to {com} (In this example, T_link_com = identity because {link} = {com}.)

	// set mass and moments of inertia of each link
	link->setMass(M, Ixx, Iyy, Izz, Ixy, Ixz, Iyz, T_link_com); // Internally, moments of inertia with respect to {link} are calculated using the moments of inertia with respect to {com} and T_link_com.

	joint->connectBodies((0!=parent)?parent->link:sim.getGround(), link);
	joint->setPosition((0!=parent)?Vec3(0.5*Lx,0,0):Vec3(0,0,0), Vec3(-0.5*Lx,0,0)); // (0,0,0) = position of joint1 in {global}, (-0.5*Lx,0,0) = position of join1 in {link1}
}

Limb::~Limb(){

}


void Limb::addChild(Limb *child){
	children.insert(child);
}

void Limb::update(float d, float dt){
	for(QSet<Limb *>::iterator i=children.begin(), e=children.end();i!=e;++i){
		Limb *l=*i;
		if(0!=l){
			l->update(d,dt);
		}
	}
}

void Limb::paint(){

	for(QSet<Limb *>::iterator i=children.begin(), e=children.end();i!=e;++i){
		Limb *l=*i;
		if(0!=l){
			l->paint();
		}
	}
}


GSystem *createGEARSystem(){

	// ground and links
	GBody *ground=new GBody;
	GBody *link1=new GBody;
	GBody *link2=new GBody;


	// joints
	GJointRevolute *joint1=new GJointRevolute;
	GJointRevolute *joint2=new GJointRevolute;

	// system (for kinematics and dynamics)
	GSystem *system=new GSystem;

	// simulation data
	struct SimulData {
			double t;
			RMatrix q, dq;
	};

	// simulated data
	std::vector<SimulData> simuldata;


	double Lx = 0.3, Ly = 0.05, Lz = 0.05; // link dimension
	double M = 1; // link mass
	double Ixx = 1./12.*M*(Ly*Ly+Lz*Lz), Iyy = 1./12.*M*(Lx*Lx+Lz*Lz), Izz = 1./12.*M*(Lx*Lx+Ly*Ly), Ixy = 0, Ixz = 0, Iyz = 0; // moments of inertia with respect to the mass center
	SE3 T_link_com = SE3(); // transformation from {link} to {com} (In this example, T_link_com = identity because {link} = {com}.)

	// set mass and moments of inertia of each link
	link1->setMass(M, Ixx, Iyy, Izz, Ixy, Ixz, Iyz, T_link_com); // Internally, moments of inertia with respect to {link} are calculated using the moments of inertia with respect to {com} and T_link_com.
	link2->setMass(M, Ixx, Iyy, Izz, Ixy, Ixz, Iyz, T_link_com);

	// set box rendering (optional)
	//	link1.setBoxRendering(Lx, Ly, Lz, SE3(), Vec3(0,0,1));
	//link2.setBoxRendering(Lx, Ly, Lz, SE3(), Vec3(1,0,0));

	// connect the links with joints
	//   Let's think about connecting two links (linkA and linkB) with a joint.
	//   A joint has two reference coordinate frames {joint left} and {joint right} which are attached to linkA and linkB respectively,
	//   and {joint left} and {joint right} are aligned with each other when the joint angle or displacement is zero.
	//   usage:
	//     joint.connectBodies(linkA, linkB) connects linkA and linkB with joint.
	//     joint.setPosition(pA, pB) sets the positions of {joint left} and {joint right} with respect to {linkA} and {linkB} respectively.
	//     joint.setOrientation(Ra, Rb) sets the orientations of {joint left} and {joint right} with respect to {linkA} and {linkB} respectively.
	joint1->connectBodies(ground, link1);
	joint1->setPosition(Vec3(0,0,0), Vec3(-0.5*Lx,0,0)); // (0,0,0) = position of joint1 in {global}, (-0.5*Lx,0,0) = position of join1 in {link1}
	joint2->connectBodies(link1, link2);
	joint2->setPosition(Vec3(0.5*Lx,0,0), Vec3(-0.5*Lx,0,0));

	// set rotating axes of the revolute joints
	joint1->setAxis(0,1,0); // rotation axis with respect to {joint left}
	joint2->setAxis(0,1,0);

	// build the two-link system
	system->buildSystem(ground); // GEAR scans the link-joint structure automatically from ground

	// set gravity
	system->setGravity(Vec3(0,0,-9.81));

	return system;

}

