#include "DeveloperWidget.hpp"
#include "ui_DeveloperWidget.h"

#include "app/Constants.hpp"
#include "gait/GaitController.hpp"
#include "hub/Hub.hpp"
#include "node/LogDevice.hpp"
#include "pose/PoseMapping.hpp"
#include "security/Key.hpp"
#include "uptime/ConnectionType.hpp"
#include "uptime/MethodGate.hpp"
#include "utility/graphics/WidgetCaptureTool.hpp"
#include "utility/random/Random.hpp"
//#include "agent/AgentConstants.hpp"


#include <QFileDialog>

DeveloperWidget::DeveloperWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::DeveloperWidget)
	, mGait(nullptr)
	, mTopDir("/")
	, mLastDir(mTopDir)
	, mWidgetIllustrationOutputDir(mTopDir)
	, mLogDevice{OC_NEW LogDeviceNoop}
	, mNodeFactory("development-")
{
	OC_METHODGATE();
	ui->setupUi(this);
}

DeveloperWidget::~DeveloperWidget()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
	delete mGait;
	mGait=nullptr;
	
}


QSharedPointer<Hub> DeveloperWidget::hub(){
	return mHub;
}

void DeveloperWidget::onListenStateChanged(const TryToggleState last, const TryToggleState current)
{
	Q_UNUSED(last);
	OC_METHODGATE();
	ui->lineEditBindPort->setEnabled(OFF==current);
	ui->comboBoxLocalAddress->setEnabled(OFF==current);
	// TODO: this should affect HubWindow
	//ui->pushButtonSendData->setEnabled(ON==current);
	const bool on=(ON==current);
	if(!on) {
		// TODO: this should affect HubWindow
		//ui->tabWidget->setCurrentWidget(ui->tabNodes);
	}
	//ui->tabWidget->setEnabled(on);
	// TODO: this should affect HubWindow
	//ui->tabNodes->setListening(on);
	//appendLog("New listening state: "+ToggleStateToSTring(current));
	if(GOING_ON==current) {
		// TODO: this should affect HubWindow
		hub()->localAddressList()->setPort(ui->lineEditBindPort->text().toInt());
		// TODO: Implement needConnection mechanism as in Remote/ClientWindow
		//		mHub->startComms();
		ui->tryToggleListen->setState(ON);
		
	} else if(OFF==current) {
		/*
		comms=mHub->comms();
		if(0!=comms) {
			//		mHub->stopComms();
		}
		*/
	}
}



void DeveloperWidget::configure(QSharedPointer<Settings> _settings, QSharedPointer<Hub> hub){
	OC_METHODGATE();
	mHub = hub;
	auto settings = _settings.data();
	ui->tabWidgetDevelopment->setCurrentWidget(ui->tabComms);
	ui->comboBoxLocalAddress->configure(settings, "hub-listen-address", "Local address");
	ui->lineEditBindPort->configure(settings, "","hub-listen-port","The port to listen for incomming netork traffic");
	ui->tryToggleListen->configure("Connect","Connecting...","Connected", "Disconnecting...", Constants::AGENT_CONNECT_BUTTON_COLOR, Constants::AGENT_DISCONNECT_COLOR);
	if(!connect(ui->tryToggleListen,SIGNAL(stateChanged(const TryToggleState, const TryToggleState)),this,SLOT(onListenStateChanged(const TryToggleState, const TryToggleState)),OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}
	
	
	//TODO: WOW we need to update this
	//		ui->widgetActuatorControl->configure(5);
	PoseMapping *pm = OC_NEW PoseMapping(5);
	ui->widgetPoseMapping->configure(*pm);
	auto h = this->hub();
	auto cmd = h->context()->commandLine();
	ui->lineEditBindPort->setText(cmd->localPortOption.valueName());
	qDebug()<<"OVERRIDING LOCAL PORT WITH VALUE FROM CMDLINE: "<<ui->lineEditBindPort->text();
	
	ui->lineEditBindPort->setValidator( OC_NEW QIntValidator(0, 65535, this) );
	
	
	ui->lineEditWidgetIlilustrationPath->setText(mWidgetIllustrationOutputDir.absolutePath());
	
	
	mGait=OC_NEW GaitController<qreal> ();
	if(nullptr!=mGait) {
		ui->widgetGait->setGait(*mGait);
	}
	
	generateTriggers();
	
}




QString DeveloperWidget::saveIdenticonWidget(IdenticonWidget *iw, QString base)
{
	QFileDialog fd;
	QStringList filters;
	QString filePath="";
	if(mTopDir == mLastDir) {
		mLastDir=QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
	}
	fd.setDirectory(mLastDir);
	filters << "PNG (*.png)";
	filters << "SVG (*.svg)";
	fd.setNameFilters(filters);
	fd.selectFile(base);
	fd.setFileMode(QFileDialog::AnyFile);
	fd.setAcceptMode(QFileDialog::AcceptSave);
	if (fd.exec()) {
		mLastDir=fd.directory();
		QStringList  fileNames = fd.selectedFiles();
		if(fileNames.size()>0) {
			QString fileName=fileNames.first();
			QFileInfo fileInfo(fileName);
			
			if(fileInfo.fileName().endsWith(".svg")) {
				QByteArray xml=iw->svgXML();
				filePath=fileInfo.absoluteFilePath();
				QFile file(filePath);
				file.open(QIODevice::WriteOnly);
				file.write(xml);
				file.close();
			} else if(fileInfo.fileName().endsWith(".png")) {
				Identicon identicon=iw->identicon();
				const quint32 sz=1024;
				QImage image=identicon.image(sz, sz);
				filePath=fileInfo.absoluteFilePath();
				image.save(filePath);
			}
		}
	}
	return filePath;
}


QString DeveloperWidget::saveIrisWidget(IrisWidget *iw, quint32 irisIndex, QString base)
{
	QFileDialog fd;
	QStringList filters;
	QString filePath="";
	if(mTopDir == mLastDir) {
		mLastDir = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
	}
	fd.setDirectory(mLastDir);
	filters << "PNG (*.png)";
	fd.setNameFilters(filters);
	fd.selectFile(base);
	fd.setFileMode(QFileDialog::AnyFile);
	fd.setAcceptMode(QFileDialog::AcceptSave);
	if (fd.exec()) {
		mLastDir=fd.directory();
		QStringList fileNames = fd.selectedFiles();
		if(fileNames.size() > 0) {
			QString fileName = fileNames.first();
			QFileInfo fileInfo(fileName);
			if(fileInfo.fileName().endsWith(".png")) {
				PortableID pid = iw->portableID();
				const quint32 sz = 1024;
				QImage irisImage(sz,sz, QImage::Format_ARGB32);
				irisImage.fill(Qt::transparent);
				QPainter painter(&irisImage);
				IrisRendrer ir;
				ir.setPortableID(pid);
				QRect r(0,0,sz,sz);
				ir.draw(r, painter, irisIndex);
				filePath=fileInfo.absoluteFilePath();
				irisImage.save(filePath);
			}
		}
	}
	return filePath;
}



void DeveloperWidget::on_horizontalSliderIrisIndex_valueChanged(int value)
{
	ui->widgetIris->setIrixIndex(value);
}



void DeveloperWidget::on_pushButtonRandomIdenticonID_clicked()
{
	OC_METHODGATE();
	QString id=Key::hash(QString::number(utility::random::qrand()));
	ui->lineEditIdenticonID->setText(id);
}


void DeveloperWidget::updateIdentityWidgets()
{
	OC_METHODGATE();
	QString id=ui->lineEditIdenticonID->text();
	PortableID pid;
	pid.setName("Dummy Identicon");
	pid.setID(id);
	pid.setType(TYPE_AGENT);
	ui->widgetIdenticonAgent->setPortableID(pid);
	ui->widgetIris->setPortableID(pid);
	ui->widgetEyes->setPortableID(pid);
	ui->widgetQR_2->setQRData(id);
	pid.setType(TYPE_REMOTE);
	ui->widgetIdenticonRemote->setPortableID(pid);
	pid.setType(TYPE_HUB);
	ui->widgetIdenticonHub->setPortableID(pid);
}





void DeveloperWidget::appendLog(const QString& text)
{
	OC_METHODGATE();
	mLogDevice->appendLog(text);
}



void DeveloperWidget::on_pushButtonSaveIdenticonAgent_clicked()
{
	const QString filePath=saveIdenticonWidget(ui->widgetIdenticonAgent, "agent_"+ui->widgetIdenticonAgent->identicon().id().id()+".svg");
	if(!filePath.isEmpty()) {
		mLogDevice->appendLog("Saved "+filePath);
	} else {
		mLogDevice->appendLog("Agent identicon save aborted");
	}
}


void DeveloperWidget::on_pushButtonSaveIdenticonRemote_clicked()
{
	const QString filePath=saveIdenticonWidget(ui->widgetIdenticonRemote, "remote_"+ui->widgetIdenticonRemote->identicon().id().id()+".svg");
	if(!filePath.isEmpty()) {
		mLogDevice->appendLog("Saved "+filePath);
	} else {
		mLogDevice->appendLog("Remote identicon save aborted");
	}
}


void DeveloperWidget::on_pushButtonSaveIdenticonHub_clicked()
{
	const QString filePath=saveIdenticonWidget(ui->widgetIdenticonHub, "hub_"+ui->widgetIdenticonHub->identicon().id().id()+".svg");
	if(!filePath.isEmpty()) {
		mLogDevice->appendLog("Saved "+filePath);
	} else {
		mLogDevice->appendLog("Hub identicon save aborted");
	}
}


void DeveloperWidget::on_pushButtonSaveIdenticonIris_clicked()
{
	const quint32 irisIndex = static_cast<quint32>(ui->horizontalSliderIrisIndex->value());
	const QString filePath=saveIrisWidget(ui->widgetIris, irisIndex, "iris_"+QString::number(irisIndex)+"_"+ui->widgetIris->portableID().id()+".png");
	if(!filePath.isEmpty()) {
		mLogDevice->appendLog("Saved "+filePath);
	} else {
		mLogDevice->appendLog("Iris image save aborted");
	}
}


void DeveloperWidget::on_pushButtonWidgetIllustrationBrowse_clicked()
{
	if(mTopDir == mWidgetIllustrationOutputDir) {
		mWidgetIllustrationOutputDir=QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
	}
	QString dir=mWidgetIllustrationOutputDir.absolutePath();
	QString selectedDir = QFileDialog::getExistingDirectory(this, tr("Select Widget Illustration output dir"), dir, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (selectedDir!=dir) {
		mWidgetIllustrationOutputDir=QDir(selectedDir);
		ui->lineEditWidgetIlilustrationPath->setText(mWidgetIllustrationOutputDir.absolutePath());
	}
}


void DeveloperWidget::on_pushButtonWidgetIllustrationSaveAll_clicked()
{
	if(mWidgetIllustrationOutputDir.exists()) {
		WidgetCaptureTool wct(palette().color(QPalette::WindowText), palette().color(QPalette::Window), QColor(0x00, 0x00, 0x00, 0x08), QColor(0x00, 0x00, 0x00, 0x00));
		wct.captureWidget(ui->widgetWidgetIllustrationIdenticon, mWidgetIllustrationOutputDir, "identicon", "Identicon widget shows rendering of the 'personality' of your agent");
		wct.captureWidget(ui->widgetWidgetIllustrationGait, mWidgetIllustrationOutputDir, "gait", "Gait widget shows a 2D graphical representation of the current gait as it progresses");
		wct.captureWidget(ui->widgetWidgetIllustrationMapEditor, mWidgetIllustrationOutputDir, "map", "Map editor widget allows user top view or edit a feature on a map such as position, area or path.");
		wct.captureWidget(ui->widgetWidgetIllustrationPairing, mWidgetIllustrationOutputDir, "pairing", "Pairing wizard allows user to pair with discovered peers on the network");
	} else {
		qWarning()<<"ERROR: Output dir for widget illustration did not exist, aborting...";
	}
}


void DeveloperWidget::generateTriggers()
{
	OC_METHODGATE();
	const auto ts = mockTriggers();
	ui->widgetTriggerManager->configure(*ts);
}


void DeveloperWidget::startConstilation(){
	OC_METHODGATE();
	const auto agentStatus = mNodeFactory.forkNodeEventloop(NodeType::TYPE_AGENT);
	const auto remoteStatus = mNodeFactory.forkNodeEventloop(NodeType::TYPE_REMOTE);
	qDebug()<<"CONSTILLATIONS"<<agentStatus<<remoteStatus;
}
