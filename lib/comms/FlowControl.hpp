#ifndef FLOWCONTROL_HPP
#define FLOWCONTROL_HPP

#include <QString>

class FlowControl{
	public:
		enum Mode{
			Good,
			Bad
		};
		Mode mode;
		float penalty_time;
		float good_conditions_time;
		float penalty_reduction_accumulator;
	public:
		FlowControl();
		void reset();
		void update( float deltaTime, float rtt );
		float sendRate() const ;

		QString getSummary(QString sep="\n")const ;

};


#endif // FLOWCONTROL_HPP
