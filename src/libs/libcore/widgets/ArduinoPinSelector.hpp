#ifndef ARDUINOPINSELECTOR_HPP
#define ARDUINOPINSELECTOR_HPP

#include <QWidget>

#include "ArduinoPinFilter.hpp"
#include "ArduinoPinMap.hpp"

class ArduinoPinFacilitator;
namespace Ui
{
class ArduinoPinSelector;
}

class ArduinoPinSelector : public QWidget
{
	Q_OBJECT

private:
	Ui::ArduinoPinSelector *ui;
	ArduinoPinFacilitator *mAPF;
	ArduinoPinFilter mFilter;
	ArduinoPinMap mPinMap;

public:
	explicit ArduinoPinSelector(QWidget *parent = 0);
	~ArduinoPinSelector();

public:

	void configure(ArduinoPinFilter filter, ArduinoPinFacilitator *apf=nullptr);

	ArduinoPin currentSelectedPin();

public slots:

	void onPinChanged();

};

#endif // ARDUINOPINSELECTOR_HPP
