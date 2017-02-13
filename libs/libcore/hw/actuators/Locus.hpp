#ifndef LOCUS_HPP
#define LOCUS_HPP

#include <QObject>


/*

  The concept of Locus is as follows:

 + You hardware may have a combination of actuator such as motors anmd servos, with an addition of limit switches and encoders. How will you tell theOctoMY™ program what is what, and how this hardware should be controlled?
 + First step is simply registering each actuator, while attaching limit switches and encoders to the actuators they belong.
 + Second step is to define sets of actuators that should work together. Each such set is referred to as a "Locus". For example if your robot is a remote control type you will first define one servo for throttle, and one for steering. Next you will define an "car" locus and attach these two servos to it.

 The locus ensures 2 things:
	1. It gives the user interface information on what kind of user control should be presented to let the user drive these actuators
	2. It gives the robot information on how the actuators should be driven autonomously

 Each locus will configure itself depending on what actuators are attached. They will also provide their own settings. For example the "car steering" locus will support one or two sets of turning wheels, and may support threads/tracks if gievn more than one motor.

 The interface between actuators and Loci is a mapping of named actuators to named loci "slots", and a separate user interface is used to maintain the mapping



*/
class Locus : public QObject
{
	Q_OBJECT
public:
	explicit Locus(QObject *parent = nullptr);



	// Locus interface

public:


	virtual QWidget *configurationWidget();

	virtual QVariantMap configuration();
	virtual void setConfiguration(QVariantMap &configuration);


signals:

	void actuatorConfigurationChanged();

};

#endif // LOCUS_HPP
