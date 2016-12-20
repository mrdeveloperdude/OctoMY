#include "IServoController.hpp"

/*
HW Descriptor for Arduino

Actuator{
enum motor_type{step, dc}
int gear_ratio_numerator, gear_ratio_denominator;
int encoder_steps;
bool potensiometer
bool tachometer
bool encoder;
bool endstop_a;
bool endstop_b;

}

*/




IServoController::IServoController(QString name, QObject *parent)
	: QObject(parent)
	, mName(name)
{

}

IServoController::~IServoController()
{

}


void IServoController::killAll()
{

}

void IServoController::centerAll()
{

}