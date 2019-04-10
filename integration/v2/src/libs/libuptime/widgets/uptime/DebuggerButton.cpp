#include "DebuggerButton.hpp"
#include "ui_DebuggerButton.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "uptime/ConnectionType.hpp"

#include "node/Node.hpp"
#include "DebuggerWidget.hpp"

DebuggerButton::DebuggerButton(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::DebuggerButton)
	, mDebuggerWidget(OC_NEW DebuggerWidget())
	, mConfigureHelper("DebuggerButton", true, false, false, true, false)
{
	OC_METHODGATE();
	ui->setupUi(this);
}

DebuggerButton::~DebuggerButton()
{
	OC_METHODGATE();
	delete ui;
}


void DebuggerButton::configure(QSharedPointer<Node> node, const bool startOpened)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mNode=node;
		mDebuggerWidget->configure(mNode);
		// Make sure to synchronize button state with actual window state
		if(!connect(mDebuggerWidget.data(), &DebuggerWidget::visibilityChanged, this, [=](bool visible) {
			ui->pushButtonShowDebugger->setChecked(visible);
		}, OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect";
		}
		if(startOpened) {
			mDebuggerWidget->show();
		}
	}

}
void DebuggerButton::on_pushButtonShowDebugger_toggled(bool checked)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		mDebuggerWidget->setVisible(checked);
	}
}
