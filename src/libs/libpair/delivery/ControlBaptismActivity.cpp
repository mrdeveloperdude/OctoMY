#include "ControlBaptismActivity.hpp"
#include "ui_ControlBaptismActivity.h"

#include "uptime/ConnectionType.hpp"
#include "uptime/MethodGate.hpp"

#include <QRegularExpression>

ControlBaptismActivity::ControlBaptismActivity(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::ControlBaptismActivity)
	, mConfigureHelper("ControlBaptismActivity", true, false, false, true, false)
{
	OC_METHODGATE();
	ui->setupUi(this);
}


ControlBaptismActivity::~ControlBaptismActivity()
{
	OC_METHODGATE();
	delete ui;
}


void ControlBaptismActivity::configureValidators(){
	const qint32 minLetters=3;
	QRegularExpression re("\\p{Ll}{"+QString::number(minLetters)+",20}");
	if(!re.isValid()) {
		qWarning()<<"ERROR: Name validator regex was invalid: "<<re.errorString();
	}
	if(!connect(ui->lineEditName, &QLineEdit::textEdited, this, [=](QString s) {
			ui->pushButtonDone->setEnabled(s.length()>=minLetters);
		}, OC_CONTYPE_NON_UNIQUE)) {
		qWarning()<<"ERROR: Could not connect";
	}
}


void ControlBaptismActivity::configure(QSharedPointer<Node> n)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		if(mNode != n) {
			mNode = n;
			configureValidators();
			reset();
		}
		if(mNode.isNull()) {
			qWarning()<<"WARNING: control baptism configured without node";
		}
	}
}


void ControlBaptismActivity::reset()
{
	OC_METHODGATE();
	randomizeName();
}


QString ControlBaptismActivity::name(){
	OC_METHODGATE();
	auto name = ui->lineEditName->text().trimmed();
	name[0] = name[0].toUpper();
	return name;
}


void ControlBaptismActivity::randomizeName(){
	OC_METHODGATE();
	ui->lineEditName->setText(mNameGenerator.generate());
}


void ControlBaptismActivity::done()
{
	OC_METHODGATE();
	pop(QStringList() << name());
}


void ControlBaptismActivity::popImpl(const QString &returnActivity, const QStringList returnArguments) {
	OC_METHODGATE();
	Q_UNUSED(returnActivity);
	Q_UNUSED(returnArguments);
}


void ControlBaptismActivity::pushImpl(const QStringList arguments) {
	OC_METHODGATE();
	Q_UNUSED(arguments);
}
