#include "LobeTypeModel.hpp"




LobeTypeModel::LobeTypeModel(QObject *parent)
	: QAbstractListModel(parent)
{
}

void LobeTypeModel::registerLobeType(const QString &nickName, const QString &fullName, const QString &iconURL)
{
	beginInsertRows(QModelIndex(), lobeTypes.count(), lobeTypes.count());
	lobeTypes.append({ nickName, fullName, iconURL });
	endInsertRows();
}

int LobeTypeModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;
	return lobeTypes.count();
}

QVariant LobeTypeModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid() || index.row() < 0 || index.row() >= lobeTypes.count()){
		return QVariant();
	}
	
	const auto &type = lobeTypes.at(index.row());
	
	switch (role){
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

QString LobeTypeModel::getFullName(const QModelIndex &index) const
{
	if (index.isValid() && index.row() < lobeTypes.count()){
		return lobeTypes.at(index.row()).fullName;
	}
	return QString();
}

QString LobeTypeModel::getNickName(const QModelIndex &index) const
{
	if (index.isValid() && index.row() < lobeTypes.count()){
		return lobeTypes.at(index.row()).nickName;
	}
	return QString();
}


void LobeTypeModel::clear()
{
	beginRemoveRows(QModelIndex(), 0, lobeTypes.count() - 1);
	lobeTypes.clear();
	endRemoveRows();
}

