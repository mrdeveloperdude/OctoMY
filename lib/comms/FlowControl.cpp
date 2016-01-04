#include "FlowControl.hpp"
#include <QDebug>
#include <QTextStream>

FlowControl::FlowControl():
	mode(Bad)
  , penalty_time(4.0f)
  , good_conditions_time(0.0f)
  , penalty_reduction_accumulator(0.0f)
{
	qDebug()<<"Flow control initialized\n";
}

void FlowControl::reset()	{
	mode = Bad;
	penalty_time = 4.0f;
	good_conditions_time = 0.0f;
	penalty_reduction_accumulator = 0.0f;
}

void FlowControl::update( float deltaTime, float rtt )	{
	const float RTT_Threshold = 250.0f;

	if ( Good==mode  ) {
		if ( rtt > RTT_Threshold ) {
			qDebug()<<"*** dropping to bad mode ***\n";
			mode = Bad;
			if ( good_conditions_time < 10.0f && penalty_time < 60.0f )				{
				penalty_time *= 2.0f;
				if ( penalty_time > 60.0f ){
					penalty_time = 60.0f;
				}
				qDebug()<<"penalty time increased to "<<penalty_time;
			}
			good_conditions_time = 0.0f;
			penalty_reduction_accumulator = 0.0f;
			return;
		}

		good_conditions_time += deltaTime;
		penalty_reduction_accumulator += deltaTime;

		if ( penalty_reduction_accumulator > 10.0f && penalty_time > 1.0f ){
			penalty_time /= 2.0f;
			if ( penalty_time < 1.0f ){
				penalty_time = 1.0f;
			}
			qDebug()<<"penalty time reduced to "<< penalty_time ;
			penalty_reduction_accumulator = 0.0f;
		}
	}

	if ( Bad==mode  )		{
		if ( rtt <= RTT_Threshold ){
			good_conditions_time += deltaTime;
		}
		else{
			good_conditions_time = 0.0f;
		}

		if ( good_conditions_time > penalty_time ){
			qDebug()<< "*** upgrading to good mode ***\n";
			good_conditions_time = 0.0f;
			penalty_reduction_accumulator = 0.0f;
			mode = Good;
			return;
		}
	}
}

float FlowControl::sendRate()const {
	return mode == Good ? 30.0f : 10.0f;
}

QString FlowControl::getSummary(QString sep) const {
	QString out;
	QTextStream ts(&out);
	ts << "sendRate: "<<sendRate()<<sep;
	ts << "mode: "<<(Good==mode?"GOOD":"BAD")<<sep;
	ts << "penaltyTime: "<<penalty_time<<sep;
	ts << "goodTime: "<<good_conditions_time<<sep;
	ts << "penaltyRed: "<<penalty_reduction_accumulator<<sep;
	return out;
}
