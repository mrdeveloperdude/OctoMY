#include "RenameDialog.hpp"
#include "ui_RenameDialog.h"

RenameDialog::RenameDialog(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::RenameDialog)
{
	ui->setupUi(this);
	
	setWindowTitle("Rename Node");
	setModal(true);
}


RenameDialog::~RenameDialog()
{
	delete ui;
}


void RenameDialog::setName(QString originalName){
	mName = originalName;
	ui->lineEditName->setText(originalName);
	ui->lineEditName->selectAll();
}


QString RenameDialog::name(){
	return mName;
}


void RenameDialog::finishRename(){
	mName = ui->lineEditName->text();
	accept();
}
