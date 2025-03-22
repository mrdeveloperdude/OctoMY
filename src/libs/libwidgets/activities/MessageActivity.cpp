#include "MessageActivity.hpp"
#include "ui_MessageActivity.h"

#include <QJsonDocument>
#include <QJsonObject>

#include "uptime/MethodGate.hpp"

MessageActivity::MessageActivity(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::MessageActivity)
{
	OC_METHODGATE();
	ui->setupUi(this);
}


MessageActivity::~MessageActivity()
{
	OC_METHODGATE();
	delete ui;
}


void MessageActivity::confirmed(){
	OC_METHODGATE();
	pop(QStringList() << mID << "true");
}


void MessageActivity::canceled(){
	OC_METHODGATE();
	pop(QStringList() << mID << "false");
}


static bool isTruthy(const QString &string){
	return "true" == string;
}


void MessageActivity::pushImpl(const QStringList arguments)
{
	OC_METHODGATE();
	QString title;
	QString bodyMD;
	QString iconPath;
	bool askConfirmation{false};
	const auto sz = arguments.size();
	if(sz > 0){
		mID = arguments[0];
	}
	if(sz > 1){
		title = arguments[1];
	}
	if(sz > 2){
		bodyMD = arguments[2];
	}
	if(sz > 3){
		iconPath = arguments[3];
	}
	if(sz > 4){
		askConfirmation = isTruthy(arguments[4]);
	}
	ui->labelTitle->setText(title);
	ui->labelBodyMD->setTextFormat(Qt::MarkdownText);
	ui->labelBodyMD->setText(bodyMD);
	ui->labelIcon->setPixmap(QPixmap(iconPath));
	ui->pushButtonCancel->setVisible(askConfirmation);
	ui->pushButtonOK->setFocus();
}


void MessageActivity::popImpl(const QString &returnActivity, const QStringList returnArguments)
{
	OC_METHODGATE();
	Q_UNUSED(returnActivity);
	Q_UNUSED(returnArguments);
/*
	if(returnArguments.size() != 1){
		qWarning() << "Invalid number of return args in pop";
		return;
	}
	auto levelString = returnArguments[0];
	auto level = trustLevelFromString(levelString);
	qDebug() << "SAVING AFTER EDIT OF " << mCurrentlyEditingID;
	auto peers = addressBook();
	if(!mNode.isNull() && !peers.isNull()) {
		if(REMOVE == level){
			peers->removeAssociate(mCurrentlyEditingID);
		}
		else{
			auto peer = peers->associateByID(mCurrentlyEditingID);
			if(peer){
				NodeType type = peer->type();
				peer->trusts().applyTrustLevel(level, type);
				qDebug() << "EDITING ENDS WITH trusts: " << peer->trusts().toStringList();
				peers->save();
			}
			else{
				qWarning() << "ERROR: No peer";
			}
		}
		//ui->listViewNodes->update();
		setCurrentPage(ui->pagePairWithPeers);
	} else {
		qWarning() << "ERROR: No node or peers while saving trust edits";
	}
	mCurrentlyEditingID = "";
*/
}
