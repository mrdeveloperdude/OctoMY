#include "IdentityAssetEditor.hpp"
#include "ui_IdentityAssetEditor.h"

#include "security/Key.hpp"
#include "uptime/MethodGate.hpp"
#include "utility/random/Random.hpp"
#include "log/LogDevice.hpp"

#include <qfiledialog.h>
#include <qpainter.h>
#include <qstandardpaths.h>

IdentityAssetEditor::IdentityAssetEditor(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::IdentityAssetEditor)
	, mTopDir("/")
	, mLastDir(mTopDir)
	, mConfigureHelper("IdentityAssetEditor", true, false, false, true)
{
	ui->setupUi(this);
}

IdentityAssetEditor::~IdentityAssetEditor()
{
	delete ui;
}


void IdentityAssetEditor::appendLog(const QString &str){
	OC_METHODGATE();
	if(mLogDevice){
		mLogDevice->appendLog(str);
	}
}

void IdentityAssetEditor::configure(QSharedPointer<Settings> _settings, QSharedPointer<Hub> hub){
	OC_METHODGATE();
	Q_UNUSED(_settings);
	if(mConfigureHelper.configure()){
		mHub = hub;
	}
}




QString IdentityAssetEditor::saveIdenticonWidget(IdenticonWidget *iw, QString base)
{
	OC_METHODGATE();
	QString filePath;
	if(mConfigureHelper.isConfiguredAsExpected()){
		QFileDialog fd;
		QStringList filters;
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
	}
	return filePath;
}


QString IdentityAssetEditor::saveIrisWidget(IrisWidget *iw, quint32 irisIndex, QString base)
{
	OC_METHODGATE();
	QString filePath;
	if(mConfigureHelper.isConfiguredAsExpected()){
		QFileDialog fd;
		QStringList filters;
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
	}
	return filePath;
}



void IdentityAssetEditor::on_horizontalSliderIrisIndex_valueChanged(int value)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()){
		ui->widgetIris->setIrixIndex(value);
	}
}



void IdentityAssetEditor::on_pushButtonRandomIdenticonID_clicked()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()){
		QString id=Key::hash(QString::number(utility::random::qrand()));
		ui->lineEditIdenticonID->setText(id);
	}
}


void IdentityAssetEditor::updateIdentityWidgets()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()){
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
}




void IdentityAssetEditor::on_pushButtonSaveIdenticonAgent_clicked()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()){
		const QString filePath=saveIdenticonWidget(ui->widgetIdenticonAgent, "agent_"+ui->widgetIdenticonAgent->identicon().id().id()+".svg");
		if(!filePath.isEmpty()) {
			appendLog("Saved "+filePath);
		} else {
			appendLog("Agent identicon save aborted");
		}
	}
}


void IdentityAssetEditor::on_pushButtonSaveIdenticonRemote_clicked()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()){
		const QString filePath=saveIdenticonWidget(ui->widgetIdenticonRemote, "remote_"+ui->widgetIdenticonRemote->identicon().id().id()+".svg");
		if(!filePath.isEmpty()) {
			appendLog("Saved "+filePath);
		} else {
			appendLog("Remote identicon save aborted");
		}
	}
}


void IdentityAssetEditor::on_pushButtonSaveIdenticonHub_clicked()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()){
		const QString filePath=saveIdenticonWidget(ui->widgetIdenticonHub, "hub_"+ui->widgetIdenticonHub->identicon().id().id()+".svg");
		if(!filePath.isEmpty()) {
			appendLog("Saved "+filePath);
		} else {
			appendLog("Hub identicon save aborted");
		}
	}
}


void IdentityAssetEditor::on_pushButtonSaveIdenticonIris_clicked()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()){
		const quint32 irisIndex = static_cast<quint32>(ui->horizontalSliderIrisIndex->value());
		const QString filePath=saveIrisWidget(ui->widgetIris, irisIndex, "iris_"+QString::number(irisIndex)+"_"+ui->widgetIris->portableID().id()+".png");
		if(!filePath.isEmpty()) {
			appendLog("Saved "+filePath);
		} else {
			appendLog("Iris image save aborted");
		}
	}
}
