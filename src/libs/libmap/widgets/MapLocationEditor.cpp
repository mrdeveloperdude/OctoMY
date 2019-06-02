#include "MapLocationEditor.hpp"
#include "ui_MapLocationEditor.h"

#include "uptime/MethodGate.hpp"

MapLocationEditor::MapLocationEditor(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::MapLocationEditor)
{
	OC_METHODGATE();
	ui->setupUi(this);
}

MapLocationEditor::~MapLocationEditor()
{
	OC_METHODGATE();
	delete ui;
}

void MapLocationEditor::on_pushButtonCancel_clicked()
{
	OC_METHODGATE();
	emit done(false);
}

void MapLocationEditor::on_pushButtonUse_clicked()
{
	OC_METHODGATE();
	emit done(true);
}
