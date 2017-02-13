#include "ActuatorSelector.hpp"



#include <QSizePolicy>

ActuatorSelector::ActuatorSelector(QWidget *parent)
	: QComboBox(parent)
{
	QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	//sizePolicy.setHeightForWidth();
	setSizePolicy(sizePolicy);
	setMinimumSize(QSize(0, 0));
	addItem("Identify Actuators...");
	addItem("Arne");
	addItem("Bjarne");
	adjustSize();
}
