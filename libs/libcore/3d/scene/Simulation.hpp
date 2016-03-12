#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include "gear/gear.h"
class Limb;

class Simulation{
	private:
		GBody *ground;
		GSystemIK *system;
		bool rebuildNeeded;
		quint64 lastTime;

	public:
		explicit Simulation();
		void rebuild();
		void addLimb(Limb *limb);
		void update();

		GBody *getGround();


};

#endif // SIMULATION_HPP
