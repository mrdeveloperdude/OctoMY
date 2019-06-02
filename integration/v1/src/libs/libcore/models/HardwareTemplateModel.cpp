#include "HardwareTemplateModel.hpp"

#include "widgets/HardwareTemplate.hpp"

#include <QIcon>
#include <QPixmap>

HardwareTemplateModel::HardwareTemplateModel(QObject *parent)
	: QAbstractListModel(parent)
	, mTemplates(HardwareTemplate::templates())
{
}

QVariant HardwareTemplateModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	//qDebug()<<" header data asked for";
	return "";
}

int HardwareTemplateModel::rowCount(const QModelIndex &parent) const
{
	//qDebug()<<" row count asked for: "<<parent;
	return mTemplates.size();
}

int HardwareTemplateModel::columnCount(const QModelIndex &) const
{
	//qDebug()<<" columncount asked for";
	return 1;
}

QVariant HardwareTemplateModel::data(const QModelIndex &index, int role) const
{
	//qDebug()<<" XXXXXXXXXXXXXXXXX Data asked for "<<index.row()<<" and role "<<role;
	if(Qt::DisplayRole ==role) {
		HardwareTemplate *tpl=mTemplates[index.row()];
		if(nullptr!=tpl) {
			QString str=tpl->name();
			return QVariant(str);
		}
	} else if (Qt::DecorationRole == role) {
		if (index.column() == 0) {
			HardwareTemplate *tpl=mTemplates[index.row()];
			if(nullptr!=tpl) {
				return QIcon(tpl->iconURL());
			}
		}
	} else if (Qt::ToolTipRole == role) {
		HardwareTemplate *tpl=mTemplates[index.row()];
		if(nullptr!=tpl) {
			return tpl->description();
		}
	}
	return QVariant();
}
