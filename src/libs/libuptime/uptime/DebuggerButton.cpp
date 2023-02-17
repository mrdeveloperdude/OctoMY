#include "DebuggerButton.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "uptime/ConnectionType.hpp"

#include "node/Node.hpp"
#include "DebuggerWidget.hpp"

DebuggerButton::DebuggerButton(QWidget *parent)
	: QPushButton(parent)
	, mDebuggerWidget(OC_NEW DebuggerWidget())
	, mConfigureHelper("DebuggerButton", true, false, false, true, false)
{
	OC_METHODGATE();
	setupUi();
}

DebuggerButton::~DebuggerButton()
{
	OC_METHODGATE();
}

void DebuggerButton::setupUi(){
	auto pushButtonShowDebugger = this;
	pushButtonShowDebugger->setObjectName("pushButtonShowDebugger");
	QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	sizePolicy.setHeightForWidth(pushButtonShowDebugger->sizePolicy().hasHeightForWidth());
	pushButtonShowDebugger->setSizePolicy(sizePolicy);
	QIcon icon;
	icon.addFile(QString::fromUtf8(":/debug/icons/debug.svg"), QSize(), QIcon::Normal, QIcon::Off);
	pushButtonShowDebugger->setIcon(icon);
	pushButtonShowDebugger->setIconSize(QSize(48, 48));
	pushButtonShowDebugger->setCheckable(true);
	pushButtonShowDebugger->setText("");
	if(!connect(pushButtonShowDebugger, &QPushButton::clicked, this, &DebuggerButton::onToggled, OC_CONTYPE)){
		qWarning()<<"Could not connect debugger buttong click";
	}
}

void DebuggerButton::configure(QSharedPointer<Node> node, const bool startOpened)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mNode=node;
		mDebuggerWidget->configure(mNode);
		// Make sure to synchronize button state with actual window state
		if(!connect(mDebuggerWidget.data(), &DebuggerWidget::visibilityChanged, this, [=](bool visible) {
			this->setChecked(visible);
		}, OC_CONTYPE_NON_UNIQUE)) {
			qWarning()<<"ERROR: Could not connect";
		}
		if(startOpened) {
			mDebuggerWidget->show();
		}
	}
}

void DebuggerButton::onToggled(bool checked)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		mDebuggerWidget->setVisible(checked);
	}
}


/*
  
#ifndef UI_DEBUGGERBUTTON_H
#define UI_DEBUGGERBUTTON_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DebuggerButton
{
public:
    QGridLayout *gridLayout;
    QPushButton *pushButtonShowDebugger;

    void setupUi(QWidget *DebuggerButton)
    {
        if (DebuggerButton->objectName().isEmpty())
            DebuggerButton->setObjectName("DebuggerButton");
        DebuggerButton->resize(146, 72);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(DebuggerButton->sizePolicy().hasHeightForWidth());
        DebuggerButton->setSizePolicy(sizePolicy);
        gridLayout = new QGridLayout(DebuggerButton);
        gridLayout->setObjectName("gridLayout");
        pushButtonShowDebugger = new QPushButton(DebuggerButton);
        pushButtonShowDebugger->setObjectName("pushButtonShowDebugger");
        sizePolicy.setHeightForWidth(pushButtonShowDebugger->sizePolicy().hasHeightForWidth());
        pushButtonShowDebugger->setSizePolicy(sizePolicy);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/debug/icons/debug.svg"), QSize(), QIcon::Normal, QIcon::Off);
        pushButtonShowDebugger->setIcon(icon);
        pushButtonShowDebugger->setIconSize(QSize(48, 48));
        pushButtonShowDebugger->setCheckable(true);

        gridLayout->addWidget(pushButtonShowDebugger, 0, 0, 1, 1);


        retranslateUi(DebuggerButton);

        QMetaObject::connectSlotsByName(DebuggerButton);
    } // setupUi

    void retranslateUi(QWidget *DebuggerButton)
    {
        DebuggerButton->setWindowTitle(QCoreApplication::translate("DebuggerButton", "Form", nullptr));
        pushButtonShowDebugger->setText(QCoreApplication::translate("DebuggerButton", "Debugger", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DebuggerButton: public Ui_DebuggerButton {};
} // namespace Ui

QT_END_NAMESPACE
*/
