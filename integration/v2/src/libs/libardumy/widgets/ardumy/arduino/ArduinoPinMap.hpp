#ifndef ARDUINOPINMAP_HPP
#define ARDUINOPINMAP_HPP

#include "ArduinoPin.hpp"
#include "ArduinoPinFilter.hpp"

#include <QVector>
#include <QStringList>

class ArduinoPinMap
{
private:

	QString mBoardName;
	QVector<ArduinoPin> mPins;
public:
	ArduinoPinMap();

	static ArduinoPinMap pinMapFactory(QString boardName);

	void verifyIntegrity();

public:

	QVector<ArduinoPin *> filter(ArduinoPinFilter &f);

	QStringList pinStringList();

	ArduinoPin pin(quint32 idx);


};

#endif // ARDUINOPINMAP_HPP
