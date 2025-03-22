#include "QRUtilityActivity.hpp"
#include "ui_QRUtilityActivity.h"

#include "node/Node.hpp"
#include "uptime/MethodGate.hpp"

#include <QJsonDocument>
#include <QJsonObject>


QRUtilityActivity::QRUtilityActivity(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::QRUtilityActivity)
{
	OC_METHODGATE();
	ui->setupUi(this);
}


QRUtilityActivity::~QRUtilityActivity()
{
	OC_METHODGATE();
	delete ui;
}

void QRUtilityActivity::configure(QSharedPointer<Node> node){
	OC_METHODGATE();
	ui->widgetQRUtility->configure(node);
}


void QRUtilityActivity::pushImpl(const QStringList arguments)
{
	OC_METHODGATE();
	Q_UNUSED(arguments);
}


void QRUtilityActivity::popImpl(const QString &returnActivity, const QStringList returnArguments)
{
	OC_METHODGATE();
	Q_UNUSED(returnActivity);
	Q_UNUSED(returnArguments);
}
