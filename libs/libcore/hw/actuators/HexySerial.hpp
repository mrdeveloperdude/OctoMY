#ifndef HEXYSERIAL_HPP
#define HEXYSERIAL_HPP


#include <QObject>

#include <QTimer>
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
		quint32 dirtyMoveFlags;
		QTimer sendTimer;

	public:
		explicit HexySerial(QObject *parent = nullptr);
		virtual ~HexySerial();

	public:
		void configure();

	public slots:
		void openSerialPort();
		void closeSerialPort();
	private slots:
		void syncMove();
		void writeData(const QByteArray &data);
		void readData();
		void dataWritten(qint64);
		void handleError(QSerialPort::SerialPortError error);
		void onSettingsChanged();

	signals:

		void settingsChanged();
		void connectionChanged();
		void readyToWrite();

	public:

		bool isConnected();

	public:

		void kill(quint32 flags=0xffffffff);
		void center();
		void version();
		void debug();
		void move(qreal pos[], quint32 flags=0xffffffff);

};

#endif // HEXYSERIAL_HPP
