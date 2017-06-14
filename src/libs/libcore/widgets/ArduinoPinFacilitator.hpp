#ifndef ARDUINOPINFACILITATOR_HPP
#define ARDUINOPINFACILITATOR_HPP

#include <QObject>
#include <QString>

class ArduinoPinFacilitator : public QObject
{
	Q_OBJECT
public:
	explicit ArduinoPinFacilitator(QObject *parent = nullptr);

public:

	bool isConnected()
	{
		return true;
	}

	QString description()
	{
		return "LOLBOB";
	}

signals:

	void pinChanged();

public slots:
};

#endif // ARDUINOPINFACILITATOR_HPP
