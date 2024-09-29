#include "ControllerTypeModel.hpp"


ControllerTypeModel::ControllerTypeModel(QObject *parent)
	: QAbstractListModel(parent)
{
}

void ControllerTypeModel::registerControllerType(const QString &nickName, const QString &fullName, const QString &iconURL)
{
	beginInsertRows(QModelIndex(), controllerTypes.count(), controllerTypes.count());
	controllerTypes.append({ nickName, fullName, iconURL });
	endInsertRows();
}

int ControllerTypeModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;
	return controllerTypes.count();
}

QVariant ControllerTypeModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid() || index.row() < 0 || index.row() >= controllerTypes.count())
		return QVariant();
	
	const ControllerType &type = controllerTypes.at(index.row());
	
	switch (role)
	{
	case Qt::DisplayRole:
		return type.fullName;
	case Qt::DecorationRole:
	{
		QIcon icon;
		icon.addFile(type.iconURL, QSize(), QIcon::Normal, QIcon::Off);
		return icon;
	}
	case Qt::ToolTipRole:
		return type.nickName;
	default:
		return QVariant();
	}
}

QString ControllerTypeModel::getFullName(const QModelIndex &index) const
{
	if (index.isValid() && index.row() < controllerTypes.count()){
		return controllerTypes.at(index.row()).fullName;
	}
	return QString();
}

QString ControllerTypeModel::getNickName(const QModelIndex &index) const
{
	if (index.isValid() && index.row() < controllerTypes.count()){
		return controllerTypes.at(index.row()).nickName;
	}
	return QString();
}
