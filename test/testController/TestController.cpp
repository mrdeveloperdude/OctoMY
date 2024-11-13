#include "TestController.hpp"

#include "hardware/controllers/ControllerFactory.hpp"
#include "hardware/controllers/IController.hpp"
#include "hardware/controllers/servotor32/Servotor32Controller.hpp"
#include "uptime/New.hpp"
#include "utility/string/QVariantPrettyPrinter.hpp"

#include <QBoxLayout>
#include <QComboBox>
#include <QFormLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QSignalSpy>
#include <QSplitter>
#include <QStackedWidget>
#include <QToolBar>
#include <QVariantMap>

void clearFormLayout(QFormLayout *layout) {
	while (QLayoutItem* item = layout->takeAt(0)) {
		if (item->widget()) {
			delete item->widget();
		}
		delete item;
	}
}


QString mapToString(const QVariantMap &map)
{
	QJsonObject jsonObject = QJsonObject::fromVariantMap(map);
	QJsonDocument jsonDoc(jsonObject);
	return jsonDoc.toJson(QJsonDocument::Indented);
}


QVariantMap stringToMap(const QString &jsonString)
{
	QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8());
	QJsonObject jsonObject = jsonDoc.object();
	return jsonObject.toVariantMap();
}


ControllerTester::ControllerTester(QWidget *parent)
	: QWidget(parent)
	, mMainLayout(OC_NEW QVBoxLayout(this))
	, mToolBar(OC_NEW QToolBar(this))
	, mDropdown(new QComboBox(this))
	, mScrollAreaUI(OC_NEW QScrollArea(this))
	, mScrollAreaForm(OC_NEW QScrollArea(this))
	, mNoControllerLabel(OC_NEW QLabel("No controller", this))
	, mNoConfigLabel(OC_NEW QLabel("No config", this))
	, mConfigEdit(OC_NEW QPlainTextEdit(this))
	, mFormLayout(OC_NEW QFormLayout())
	, mFormWidget(OC_NEW QWidget(this))
	, mStackedConfigLabel(OC_NEW QStackedWidget(this))
	, mSplitter(OC_NEW QSplitter(Qt::Horizontal, this))
{
	mNoControllerLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	mNoConfigLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	mScrollAreaUI->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	auto boldFont = mNoConfigLabel->font();
	boldFont.setBold(true);
	mNoControllerLabel->setFont(boldFont);
	mNoConfigLabel->setFont(boldFont);
	mNoControllerLabel->setEnabled(false);
	mNoConfigLabel->setEnabled(false);
	ControllerFactory factory;
	mDropdown->addItems(factory.availableControllers());
	mToolBar->addWidget(mDropdown);
	QAction *allocatedAction = new QAction("Allocated", this);
	allocatedAction->setCheckable(true);
	mToolBar->addAction(allocatedAction);
	QAction *debugAction = new QAction("Debug Mode", this);
	debugAction->setCheckable(true);
	mToolBar->addAction(debugAction);
	QAction *connectedAction = new QAction("Connected", this);
	connectedAction->setCheckable(true);
	mToolBar->addAction(connectedAction);
	
	QAction *readConfigAction = new QAction("Read Config", this);
	mToolBar->addAction(readConfigAction);
	QAction *writeConfigAction = new QAction("Write Config", this);
	mToolBar->addAction(writeConfigAction);
	
	mScrollAreaUI->setWidget(mNoControllerLabel);
	mScrollAreaUI->setWidgetResizable(true);
	mScrollAreaForm->setWidget(mFormWidget);
	mScrollAreaForm->setWidgetResizable(true);
	mConfigEdit->setLineWrapMode(QPlainTextEdit::WidgetWidth);
	mFormWidget->setLayout(mFormLayout);
	mSplitter->addWidget(mScrollAreaUI);
	mStackedConfigLabel->addWidget(mConfigEdit);
	mStackedConfigLabel->addWidget(mNoConfigLabel);
	mSplitter->addWidget(mStackedConfigLabel);
	mSplitter->addWidget(mScrollAreaForm);
	mMainLayout->addWidget(mToolBar);
	mMainLayout->addWidget(mSplitter);
	setLayout(mMainLayout);
	updateWidget();
	updateConfigInfo();
	updateActuatorInfo();
	connect(debugAction, &QAction::toggled, this, &ControllerTester::toggleDebug);
	connect(connectedAction, &QAction::toggled, this, &ControllerTester::toggleConnection);
	connect(allocatedAction, &QAction::toggled, this, &ControllerTester::toggleAllocation);
	connect(readConfigAction, &QAction::triggered, this, &ControllerTester::readConfig);
	connect(writeConfigAction, &QAction::triggered, this, &ControllerTester::writeConfig);
}


void ControllerTester::toggleDebug(bool wantsDebug) {
	const auto hasDebug = nullptr != mController && mController->isDebug();
	if(wantsDebug != hasDebug){
		if(mController){
			mConfigEdit->setPlainText(mapToString(mController->configuration()));
			mController->setDebug(wantsDebug);
		}
	}
	updateWidget();
	updateConfigInfo();
	updateActuatorInfo();
}


void ControllerTester::toggleConnection(bool wantsConnection) {
	const auto hasConnection = nullptr != mController && mController->isConnected();
	if(wantsConnection != hasConnection){
		if(mController){
			mController->setConnected(wantsConnection);
		}
	}
	updateWidget();
	updateConfigInfo();
	updateActuatorInfo();
}


void ControllerTester::toggleAllocation(bool wantsAllocated) {
	if(wantsAllocated != mAllocated){
		if (wantsAllocated) {
			mDropdown->setEnabled(false);
			// NOTE: We don't use the factory here precicely because we are testing
			switch(mDropdown->currentIndex()){
			case(1):{
				mController = OC_NEW Servotor32Controller();
			}break;
#ifdef OC_USE_FEATURE_ARDUINO
			case(2):{
				mController = OC_NEW ArduMYController();
			}break;
#endif
			}
		} else {
			if(mController){
				mController->deleteLater();
				mController = nullptr;
			}
			mDropdown->setEnabled(true);
		}
		updateWidget();
		updateConfigInfo();
		updateActuatorInfo();
		mAllocated = wantsAllocated;
	}
	
}


void ControllerTester::readConfig(){
	if(mController){
		mConfigEdit->setPlainText(mapToString(mController->configuration()));
	}
	updateConfigInfo();
}


void ControllerTester::writeConfig(){
	if(mController){
		mController->setConfiguration(stringToMap(mConfigEdit->toPlainText()));
	}
	updateConfigInfo();
}

void ControllerTester::configChanged(){
	qDebug()<<"CONFIG CHANGED";
}


void ControllerTester::updateWidget() {
	QWidget *widget{nullptr};
	if(mController){
		widget = mController->configurationWidget();
		if (widget) {
			auto controller = qobject_cast<IController *>(widget);
			if (controller) {
				connect(controller, &IController::configurationChanged, this, &ControllerTester::configChanged);
			}
		}
		else{
			widget = mNoControllerLabel;
		}
	}
	if(widget){
		widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
		widget->setMinimumSize(100, 100);
		widget->adjustSize();
	}
	mScrollAreaUI->setWidget(widget);
}


void ControllerTester::updateConfigInfo() {
	mStackedConfigLabel->setCurrentWidget(mController?static_cast<QWidget *>(mConfigEdit):static_cast<QWidget *>(mNoConfigLabel));
}


void ControllerTester::updateActuatorInfo() {
	clearFormLayout(mFormLayout);
	if (mController) {
		mFormLayout->addRow("Max Actuators Supported:", new QLabel(QString::number(mController->maxActuatorsSupported())));
		mFormLayout->addRow("Actuator count:", new QLabel(QString::number(mController->actuatorCount())));
		for (quint8 i = 0; i < mController->actuatorCount(); ++i) {
			mFormLayout->addRow(QString("  Actuator %1:").arg(i), new QLabel(mController->actuatorName(i)));
			mFormLayout->addRow(QString("  Value:"), new QLabel(QString::number(mController->actuatorTargetValue(i))));
			mFormLayout->addRow(QString("  Default:"), new QLabel(QString::number(mController->actuatorDefaultValue(i))));
		}
		mFormLayout->addRow("Title:", new QLabel(mController->controllerTitle()));
		mFormLayout->addRow("Description:", new QLabel(mController->controllerDescription()));
		mFormLayout->addRow("Icon:", new QLabel(mController->controllerIcon()));
		mFormLayout->addRow("Version 1:", new QLabel(mController->controllerVersion()));
		mFormLayout->addRow("Version 2:", new QLabel(mController->version()));
		mFormLayout->addRow("Debug:", new QLabel(mController->isDebug()?"ENABLED":"DISABLED"));
		mFormLayout->addRow("Connected:", new QLabel(mController->isConnected()?"CONNECTED":"DISCONNECTED"));
		mFormLayout->addRow("Config UI:", new QLabel(mController->hasConfigurationWidget()?"YES":"NO"));
		mFormLayout->addRow("Debug String:", new QLabel(mController->debugString()));
	}
}


void TestController::test()
{
	auto tester = OC_NEW ControllerTester;
	tester->setAttribute(Qt::WA_DeleteOnClose);
	tester->show();
	QSignalSpy spy(tester, &ControllerTester::destroyed);
	spy.wait(1000000);
}


OC_TEST_MAIN(test, TestController)
