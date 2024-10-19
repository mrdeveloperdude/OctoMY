#include "Servotor32Controller.hpp"

#include "hardware/serial/SerialSettings.hpp"
#include "hardware/controllers/servotor32/Servotor32ControllerWidget.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "uptime/ConnectionType.hpp"

#include <QDebug>
#include <QtGlobal>
#include <QStringBuilder>
#include <QBitArray>
#include <QSerialPort>

static const QString sepStr{"\n\r"};
static const size_t sepStrSz = sepStr.size();

static const QByteArray sepBa(sepStr.toLatin1());
static const size_t sepBaSz = sepBa.size();

const int Servotor32Controller::SERVO_COUNT{32};

Servotor32Controller::Servotor32Controller(QObject *parent)
	: IController("Servotor32", "ArcBotics Servotor32", ":/icons/arcbotics_logo.svg", parent)
	, mReads(0)
	, mWidget(nullptr)
	, mSerialInterface(OC_NEW QSerialPort(this))
	, mConfigureHelper("Servotor32Controller", true, true, false)
{
	OC_METHODGATE();
	qRegisterMetaType<QSerialPort::SerialPortError>();
	mSerialSettings = s32_defaultSerialSettings();
	s32_setHookSerialSignals(true);
	// TODO: Fix this:
	mConfigureHelper.configure();
}


Servotor32Controller::~Servotor32Controller()
{
	OC_METHODGATE();
	if(isConnected()) {
		// ASIMOV: Limp all actuators before closing shop to avoid frying them if they are trying to reach impossible positions
		limpAll();
		s32_closeSerialPort();
	}
	s32_setHookSerialSignals(false);
}


// Servotor32 related
//////////////////////////////////////////////


SerialSettings Servotor32Controller::s32_defaultSerialSettings(){
	OC_METHODGATE();
	// From https://arcbotics.com/lessons/servotor32-commands/
	return SerialSettings("Servotor32Default", QSerialPort::Baud9600, QSerialPort::Data8, QSerialPort::NoParity, QSerialPort::OneStop);
}


void Servotor32Controller::s32_configureSerial(SerialSettings &serialSettings)
{
	OC_METHODGATE();
	qDebug() << "Servotor32Controller configured with new serial settings: " << serialSettings;
	const auto wasConnected{isConnected()};
	if(wasConnected){
		s32_closeSerialPort();
	}
	mSerialSettings = serialSettings;
	if(wasConnected){
		s32_openSerialPort();
	}
}


void Servotor32Controller::s32_openSerialPort()
{
	OC_METHODGATE();
	auto p = mSerialSettings;
	mSerialInterface->setPortName(p.name);
	mSerialInterface->setBaudRate(p.baudRate);
	mSerialInterface->setDataBits(p.dataBits);
	mSerialInterface->setParity(p.parity);
	mSerialInterface->setStopBits(p.stopBits);
	mSerialInterface->setFlowControl(p.flowControl);
	if (mSerialInterface->open(QIODevice::ReadWrite)) {
		qDebug() << tr("Connected to %1").arg(serialSettingsToString(p));
		s32_writeData("\n");
		s32_version();
		emit connectionChanged();
	} else {
		qDebug() << "ERROR OPENING: " << serialSettingsToString(p) << ":" << mSerialInterface->errorString();
	}
}


void Servotor32Controller::s32_closeSerialPort()
{
	OC_METHODGATE();
	if (mSerialInterface->isOpen()) {
		mSerialInterface->close();
		// Forget last version because next time we connect there might be a different hardware with different version
		mLastSerialSideVersion = "";
		emit connectionChanged();
	}
	qDebug() << "Disconnected";
}


void Servotor32Controller::s32_setHookSerialSignals(bool hook)
{
	OC_METHODGATE();
	if(hook) {
		if(!connect(mSerialInterface, &QSerialPort::errorOccurred, this, &Servotor32Controller::onSerialHandleError)) {
			qWarning() << "ERROR: Could not connect";
		}
		if(!connect(mSerialInterface, &QIODevice::readyRead, this, &Servotor32Controller::onSerialReadData)) {
			qWarning() << "ERROR: Could not connect";
		}
		if(!connect(mSerialInterface, &QIODevice::bytesWritten, this, &Servotor32Controller::onSerialDataWritten)) {
			qWarning() << "ERROR: Could not connect";
		}
	} else {
		if(!disconnect(mSerialInterface, &QSerialPort::errorOccurred, this, &Servotor32Controller::onSerialHandleError)) {
			qWarning() << "ERROR: Could not disconnect";
		}
		if(!disconnect(mSerialInterface, &QIODevice::readyRead, this, &Servotor32Controller::onSerialReadData)) {
			qWarning() << "ERROR: Could not disconnect";
		}
		if(!disconnect(mSerialInterface, &QIODevice::bytesWritten, this, &Servotor32Controller::onSerialDataWritten)) {
			qWarning() << "ERROR: Could not disconnect";
		}
	}
}

// NOTE: This will carry out the actual writing when serial signals there is an opportunity.
//	     The data for move is accumulated and consolidated by one or more move() commands.
void Servotor32Controller::s32_syncMove()
{
	OC_METHODGATE();
	if(isConnected()) {
		QString data;
		//qDebug() << "SYNC-MOVE: "<< QString("%1").arg( dirtyMoveFlags, 16, 2, QChar('0'));
		const int sz=mAccumulatedPosition.size();
		for(int i=0; i<sz; ++i) {
			if(mDirtyMoveFlags.testBit(i)) {
				data += "#" +QString::number(i) + "P" + QString::number(mAccumulatedPosition[i]) +"\n";
			}
		}
		s32_writeData(data.toLatin1());
		mDirtyMoveFlags.fill(false);
	} else {
		qWarning() << "ERROR: Trying to syncMove with serial when not connected";
	}
}


void Servotor32Controller::s32_writeData(const QByteArray &data)
{
	OC_METHODGATE();
	if(0==data.size()) {
		qWarning() << "ERROR: WRITING CALLED WITH NO DATA";
	} else {
		const qint64 accepted=mSerialInterface->write(data);
		if(accepted<0) {
			qWarning() << "ERROR: write was not accepted. ERROR="<<mSerialInterface->errorString();
		} else {
			if(data.size()>accepted) {
				qWarning() << "ERROR: Data truncation occurred, serial port would not accept more";
			}
			//serial->flush();
			//qDebug() << "WRITING: "<<data.size() << " accepted "<<accepted;
		}
	}
}

void Servotor32Controller::s32_version(){
	OC_METHODGATE();
	if(isConnected()) {
		mAskedVersion = true;
		s32_writeData("V\n");
	} else {
		qWarning() << "ERROR: Trying to get version via serial when not connected";
	}
	
}


void Servotor32Controller::s32_debug()
{
	OC_METHODGATE();
	if(isConnected()) {
		s32_writeData("D\n");
	} else {
		qWarning() << "ERROR: Trying to debug with serial when not connected";
	}
	
}


// Serial IO slots
//////////////////////////////////////////////


void Servotor32Controller::onSerialReadData()
{
	OC_METHODGATE();
	if(isConnected()) {
		QByteArray data = mSerialInterface->readAll();
		mInputBuffer.append(data);
		const int lio=mInputBuffer.indexOf(sepBa);
		mReads++;
		// Output non-empty lines
		if(lio > 0) {
			QByteArray part = mInputBuffer.left(lio);
			mInputBuffer = mInputBuffer.mid(lio + sepBaSz);
			qDebug() << QString::fromLatin1(part) << " (over " << mReads << " reads)";
			mReads = 0;
			if(mAskedVersion){
				mAskedVersion = false;
				mLastSerialSideVersion = part;
			}
		}
		// Simply skip empty lines
		else if(0 == lio) {
			qDebug() << "SKIPPING EMPTY SERIAL IO READ";
			mInputBuffer = mInputBuffer.mid(sepBaSz);
		}
		//TODO:parse and handle data
	} else {
		qWarning() << "ERROR: Trying to read data from serial when not connected";
	}
}


void Servotor32Controller::onSerialDataWritten( qint64 written)
{
	OC_METHODGATE();
	Q_UNUSED(written);
	qint64 left=mSerialInterface->bytesToWrite();
	//qDebug() << "DATA WRITTEN: "<<written <<" (left:"<<left<<") ";
	if(left<=0 && mDirtyMoveFlags.count(true)) {
		s32_syncMove();
	}
}


void Servotor32Controller::onSerialHandleError(QSerialPort::SerialPortError error)
{
	OC_METHODGATE();
	mLastSerialError = mSerialInterface->errorString();
	
	switch (error) {
	case QSerialPort::NoError:{
			// No action needed for no error.
		}break;
			
		case QSerialPort::ResourceError:
		case QSerialPort::DeviceNotFoundError:
		case QSerialPort::PermissionError:
		case QSerialPort::OpenError:{
			// Treat these errors as critical and close the port
			qDebug() << "SERVOTOR32 Critical serial error:" << mLastSerialError << ", closing";
			s32_closeSerialPort();
		}break;
			
		case QSerialPort::ReadError:
		case QSerialPort::WriteError:{
			// These errors are less critical but still signal communication issues.
			qDebug() << "SERVOTOR32 Communication error " << error << ": " << mLastSerialError;
		}break;
			
		case QSerialPort::TimeoutError:{
			// Handle timeout errors differently if necessary
			qDebug() << "SERVOTOR32 Timeout error: " << mLastSerialError;
		}break;
			
		case QSerialPort::UnsupportedOperationError:{
			// Handle unsupported operations, maybe just a warning
			qDebug() << "SERVOTOR32 Unsupported operation: " << mLastSerialError;
		}break;
			
		case QSerialPort::NotOpenError:{
			// Not open errors could indicate incorrect usage
			qDebug() << "SERVOTOR32 Port not open: " << mLastSerialError;
		}break;
			
		default:
		case QSerialPort::UnknownError:{
			// Any other unhandled errors
			qDebug() << "SERVOTOR32 Unhandled/unknown serial error: " << mLastSerialError;
		}break;
	}
}


void Servotor32Controller::onSerialSettingsChanged()
{
	OC_METHODGATE();
	{
		QSignalBlocker sb(this);
		s32_closeSerialPort();
		s32_openSerialPort();
	}
	emit settingsChanged();
}


// IController interface
//////////////////////////////////////////////



QString Servotor32Controller::version()
{
	OC_METHODGATE();
	return mLastSerialSideVersion;
}


bool Servotor32Controller::hasConfigurationWidget() const{
	OC_METHODGATE();
	return true;
}


QWidget *Servotor32Controller::configurationWidget()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()){
		if(nullptr == mWidget) {
			mWidget = OC_NEW Servotor32ControllerWidget(nullptr);
			if(nullptr != mWidget) {
				mWidget->configure(this);
			}
		}
	}
	return mWidget;
}


QVariantMap Servotor32Controller::configuration(){
	OC_METHODGATE();
	QVariantMap map;
	map["serial"] = mSerialSettings.toMap();
	return map;
}


void Servotor32Controller::setConfiguration(const QVariantMap &map){
	OC_METHODGATE();
	auto serial = s32_defaultSerialSettings();
	if(map.contains("serial")){
		serial.fromMap(map["serial"].toMap());
	}
	mSerialSettings = serial;
	emit settingsChanged();
}


void Servotor32Controller::setConnected(bool open)
{
	OC_METHODGATE();
	qDebug() << "Status was" << isConnected();
	if(open) {
		s32_openSerialPort();
		qDebug() << "OPENED servotor32 version " << version();
	} else {
		qDebug() << "CLOSING servotor32 version " << version();
		s32_closeSerialPort();
	}
	qDebug() << "Status is" << isConnected();
}


bool Servotor32Controller::isConnected()
{
	OC_METHODGATE();
	return mSerialInterface->isOpen();
}


quint8 Servotor32Controller::maxActuatorsSupported()
{
	OC_METHODGATE();
	return SERVO_COUNT;
}


quint8 Servotor32Controller::actuatorCount()
{
	OC_METHODGATE();
	return static_cast<quint8>(mAccumulatedPosition.size());
}


QString Servotor32Controller::actuatorName(quint8)
{
	OC_METHODGATE();
	//TODO: Implement
	return "IMPLEMENT ME!";
}


qreal Servotor32Controller::actuatorValue(quint8 index)
{
	OC_METHODGATE();
	if(index>=mAccumulatedPosition.size()) {
		return 0.0;
	}
	return mAccumulatedPosition[index];
}


qreal Servotor32Controller::actuatorDefault(quint8 )
{
	OC_METHODGATE();
	//There is no concept of default value so we simply say center
	return 0.5;
}


void Servotor32Controller::limp(quint8 index, bool limp)
{
	OC_METHODGATE();
	if(isConnected()) {
		if(index>actuatorCount()) {
			return;
		}
		QString data;
		if(limp) {
			data += QString::fromLatin1("#") +  QString::number(index) + QString::fromLatin1("L\n");
		} else {
			data += QString::fromLatin1("#") +  QString::number(index) + QString::fromLatin1("P") + QString::number(mAccumulatedPosition[index]) + QString::fromLatin1("\n");
		}
		qDebug() << "LIMP SINGLE: "<<data;
		s32_writeData(data.toLatin1());
	} else {
		qWarning() << "ERROR: Trying to limp single atuator via serial when not connected";
	}
}


void Servotor32Controller::move(quint8 i, qreal value)
{
	OC_METHODGATE();
	const quint32 p=(qBound(-1.0, value, 1.0)*1000.0+1500.0);
	// Skip unecessary communication if value did not change
	// TODO: Fix float vs int problem
	if(mAccumulatedPosition[i]!=p) {
		mAccumulatedPosition[i]=p;
		if(0 == mSerialInterface->bytesToWrite()) {
			s32_syncMove();
		}
	}
}


void Servotor32Controller::limp(const QBitArray &flags)
{
	OC_METHODGATE();
	//Trivial reject: limp ALL
	if(isConnected()) {
		const int sz=flags.size();
		if(flags.count(true)==sz) {
			limpAll();
		} else {
			QString data;
			for(int i=0; i<sz; ++i) {
				if(flags.testBit(i)) {
					data += QString::fromLatin1("#") +  QString::number(i) + QString::fromLatin1("L\n");
				} else {
					data += QString::fromLatin1("#") +  QString::number(i) + QString::fromLatin1("P") + QString::number(mAccumulatedPosition[i]) + QString::fromLatin1("\n");
				}
			}
			qDebug() << "LIMP: "<<data<<" for "<<flags;
			s32_writeData(data.toLatin1());
		}
	} else {
		qWarning() << "ERROR: Trying to limp subset of actuators via serial when not connected";
	}
}


// NOTE: This will simply collect the latest data. The actual writing is done in s32_syncMove when serial signals there is an opportunity.
// TODO: look at binary extension introduced in 2.1 version of hexy firmware to improve performance
void Servotor32Controller::move(const Pose &pose)
{
	OC_METHODGATE();
	//qreal pos[1]= {0.0};	quint32 flags;
	if(isConnected()) {
		const int sz=static_cast<int>(pose.size());
		if(mAccumulatedPosition.size()<sz) {
			mAccumulatedPosition.resize(sz);
			mDirtyMoveFlags.resize(sz);
		}
		for(int i=0; i<sz; ++i) {
			// TODO: Fix float vs int issue
			const quint32 p = (qBound(-1.0, pose.value(i), 1.0)*1000.0+1500.0);
			//Skip unecessary communication if value did not change
			if(!qFuzzyCompare(mAccumulatedPosition[i], p)) {
				mAccumulatedPosition[i]=p;
				mDirtyMoveFlags.setBit(i);
			}
		}
		//qDebug() << "MOVE: flags="<< QString("%1").arg( flags, 16, 2, QChar('0'))<< ", dirtyMoveFlags="<<QString("%1").arg( dirtyMoveFlags, 16, 2, QChar('0'));
		if(mDirtyMoveFlags.count(true)>0 && 0==mSerialInterface->bytesToWrite()) {
			s32_syncMove();
		}
	} else {
		qWarning() << "ERROR: Trying to move actuators with serial when not connected";
	}
}


void Servotor32Controller::limpAll()
{
	OC_METHODGATE();
	if(isConnected()) {
		s32_writeData("K\n");
	} else {
		qWarning() << "ERROR: Trying to limp all actuators via serial when not connected";
	}
}


void Servotor32Controller::centerAll()
{
	OC_METHODGATE();
	if(isConnected()) {
		s32_writeData("C\n");
	} else {
		qWarning() << "ERROR: Trying to center actuators using serial when not connected";
	}
}


QString Servotor32Controller::debugString(){
	OC_METHODGATE();
	QString ret;
	ret+=QString("Last serial settings: %1\n").arg(serialSettingsToString(mSerialSettings));
	ret+=QString("Is connected: %1\n").arg(isConnected()?"YES":"NO");
	ret+=QString("Last version: %1\n").arg(mLastSerialSideVersion);
	ret+=QString("Last serial error: %1\n").arg(mLastSerialError);
	return ret;
}
