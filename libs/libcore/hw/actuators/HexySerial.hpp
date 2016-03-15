#ifndef HEXYSERIAL_HPP
#define HEXYSERIAL_HPP


#include <QObject>


#include <QtSerialPort/QSerialPort>



class SerialSettings;

class HexySerial : public QObject{
		Q_OBJECT
	public:
		const static quint32 SERVO_COUNT=32;
	private:

		SerialSettings *settings;
		QSerialPort *serial;
		QByteArray inBuf;
		quint32 lastPos[SERVO_COUNT];

	public:
		explicit HexySerial(QObject *parent = 0);
		virtual ~HexySerial();


	private slots:
		void openSerialPort();
		void closeSerialPort();
		void writeData(const QByteArray &data);
		void readData();
		void handleError(QSerialPort::SerialPortError error);
		void onSettingsChanged();

	signals:

		void settingsChanged();

	public:

		bool isConnected();
	public:

		void configure();
		void kill(quint32 flags=0xffffffff);
		void center();
		void version();
		void debug();
		void move(qreal pos[], quint32 flags=0xffffffff);

};

#endif // HEXYSERIAL_HPP
