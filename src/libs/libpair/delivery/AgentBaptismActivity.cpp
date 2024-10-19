#include "AgentBaptismActivity.hpp"
#include "name/GenderGenerator.hpp"
#include "node/Node.hpp"
#include "security/PortableID.hpp"
#include "ui_AgentBaptismActivity.h"

#include "uptime/ConnectionType.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"


AgentBaptismActivity::AgentBaptismActivity(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::AgentBaptismActivity)
	, mConfigureHelper("AgentBaptismActivity", true, false, false, true, false)
{
	OC_METHODGATE();
	ui->setupUi(this);
}

AgentBaptismActivity::~AgentBaptismActivity()
{
	OC_METHODGATE();
	delete ui;
}


void AgentBaptismActivity::configureValidators(){
	const qint32 minLetters=3;
	QRegularExpression re("\\p{Ll}{"+QString::number(minLetters)+",20}");
	if(!re.isValid()) {
		qWarning()<<"ERROR: Name validator regex was invalid: "<<re.errorString();
	}
	//Regex rules from http://stackoverflow.com/questions/38001256/handling-accented-letters-in-qregularexpressions-in-qt5/38001274#38001274
	//             and http://www.regular-expressions.info/unicode.html
	ui->lineEditName->setValidator(OC_NEW QRegularExpressionValidator(re, this));
	if(!connect(ui->lineEditName, &QLineEdit::textEdited, this, [this](QString s) {
			ui->pushButtonDone->setEnabled(s.length() >= minLetters);
		}, OC_CONTYPE_NON_UNIQUE)) {
		qWarning()<<"ERROR: Could not connect";
	}
}


void AgentBaptismActivity::configure(QSharedPointer<Node> n)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		if(mNode != n) {
			mNode = n;
			configureValidators();
			reset();
		}
		if(mNode.isNull()) {
			qWarning()<<"WARNING: agent baptism configured without node";
		}
	}
}


void AgentBaptismActivity::reset()
{
	OC_METHODGATE();
	ui->lineEditName->setText(mNameGenerator.generate());
	ui->comboBoxGender->setCurrentIndex(0);
	PortableID id;
	QString name, gender;
	if(!mNode.isNull()){
		auto nodeIdentity = mNode->nodeIdentity();
		if(!nodeIdentity.isNull()){
			id = nodeIdentity->toPortableID();
			name = id.name();
			gender = id.gender();
		}
	}
	if(name.trimmed().isEmpty()){
		randomizeName();
		ui->lineEditName->selectAll();
	}
	else{
		ui->lineEditName->setText(name);
	}
	if(gender.trimmed().isEmpty()){
		randomizeGender();
	}
	else{
		ui->comboBoxGender->setCurrentText(gender);
	}
}


QString AgentBaptismActivity::name(){
	OC_METHODGATE();
	auto name = ui->lineEditName->text().trimmed();
	name[0] = name[0].toUpper();
	return name;
}


QString AgentBaptismActivity::gender(){
	OC_METHODGATE();
	auto gender = 0 > ui->comboBoxGender->currentIndex()?ui->comboBoxGender->currentText():mGenderGenerator.generate();
	return gender;
}


void AgentBaptismActivity::randomizeName()
{
	OC_METHODGATE();
	ui->lineEditName->setText(mNameGenerator.generate(true));
}


void AgentBaptismActivity::randomizeGender()
{
	OC_METHODGATE();
	ui->comboBoxGender->setCurrentIndex((rand()%(ui->comboBoxGender->count()-1))+1);
}


void AgentBaptismActivity::done()
{
	OC_METHODGATE();
	pop(QStringList() << name() << gender());
}


void AgentBaptismActivity::popImpl(const QString &returnActivity, const QStringList returnArguments) {
	OC_METHODGATE();
	Q_UNUSED(returnActivity);
	Q_UNUSED(returnArguments);
}


void AgentBaptismActivity::pushImpl(const QStringList arguments) {
	OC_METHODGATE();
	Q_UNUSED(arguments);
}
