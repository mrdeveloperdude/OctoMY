#include "Simulation.hpp"

#include <QDebug>
#include <QDateTime>

Simulation::Simulation()
	: ground(new GBody)
	, system(new GSystemIK)
	, rebuildNeeded(true)
	, lastTime(QDateTime::currentMSecsSinceEpoch())

{
	system->buildSystem(ground);
	system->setGravity(Vec3(0,0,-9.81));
}


void Simulation::rebuild(){
	if(rebuildNeeded){
		system->buildSystem(ground);
		system->initBodyForcesAndJointTorques();
		rebuildNeeded=false;
	}
}


void Simulation::addLimb(Limb *limb){
	if(0!=limb){
		rebuildNeeded=true;
	}
}

void Simulation::update(){
	const quint64 now=QDateTime::currentMSecsSinceEpoch();
	const quint64 interval=now-lastTime;
	lastTime=now;
	const qreal stepSize=((qreal)interval)/1000.0;
	rebuild();
	system->initBodyForcesAndJointTorques();
	if(!system->stepSimulation(stepSize)){
		qDebug()<<"simulation step failed";
	}
}



GBody *Simulation::getGround(){
	return ground;
}
