#include "RemoteController.hpp"
#include "remote/Remote.hpp"
#include "ui_RemoteController.h"

RemoteController::RemoteController(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::RemoteController)
	, mConfigureHelper("RemoteController", true, false, true, true, false)
{
	ui->setupUi(this);
}

RemoteController::~RemoteController()
{
	delete ui;
}




void RemoteController::configure(QSharedPointer<Remote> remote){
	if(mConfigureHelper.configure()){
		mRemote = remote;
		if(!mRemote.isNull()) {
			/*
			if(!connect(mRemote.data(), &Node::identityChanged, this, [this]() {
					updateStage();
				},OC_CONTYPE_NON_UNIQUE)) {
				qWarning()<<"ERROR: Could not connect";
			}
*/
		}
	}
}


