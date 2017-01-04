#include "ArduinoPinSelector.hpp"

#include <QSizePolicy>

ArduinoPinSelector::ArduinoPinSelector(QWidget *parent)
	: QComboBox(parent)
{
	QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	//sizePolicy.setHeightForWidth();
	setSizePolicy(sizePolicy);
	setMinimumSize(QSize(0, 0));
	addItem("Identify Pins...");
	addItem("Pin 0  DIGITAL  [ I O ]");
	addItem("Pin 1  Analogue [   O ] 10-bit");
	addItem("Pin 2  Analogue [   O ]  8-bit");
	addItem("Pin 3  DIGITAL  [ I   ]");
	adjustSize();
}
