#include "ArduinoPinCombobox.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include "widgets/ArduinoPinMap.hpp"

#include <QSizePolicy>

ArduinoPinCombobox::ArduinoPinCombobox(QWidget *parent)
	: QComboBox(parent)
{
	OC_METHODGATE();
	QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	//sizePolicy.setHeightForWidth();
	setSizePolicy(sizePolicy);
	setMinimumSize(QSize(0, 0));

	addItem("Identify Pins...");
	addItems(ArduinoPinMap::pinMapFactory("mega2560").pinStringList());
	adjustSize();
	/*
		connect(this, &ArduinoPinSelector::currentIndexChanged, [=](int index){
			emit arduinoPinChanged(index);

		}, OC_CONTYPE);
		*/
}
