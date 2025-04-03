#include "InternalResourceManager.hpp"
#include "ui_InternalResourceManager.h"

#include "manager/ResourceTreeFilterProxyModel.hpp"
#include "manager/ResourceTreeModel.hpp"
#include "uptime/New.hpp"

#include <QRegularExpression>

InternalResourceManager::InternalResourceManager(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::InternalResourceManager)
	, mModel{OC_NEW ResourceTreeModel(this)}
	, mProxyModel{OC_NEW ResourceTreeFilterProxyModel(this)}
{
	ui->setupUi(this);
	mProxyModel->setSourceModel(mModel);
	ui->treeViewResources->setModel(mProxyModel);
}

InternalResourceManager::~InternalResourceManager()
{
	delete ui;
}


void InternalResourceManager::search(const QString &filter){
	mProxyModel->setFilterFixedString(filter);
}
