#ifndef CONTROLLERTYPEMODEL_HPP
#define CONTROLLERTYPEMODEL_HPP

#include <QAbstractListModel>


#include <QAbstractListModel>
#include <QIcon>
#include <QString>

class ControllerTypeModel : public QAbstractListModel
{
	Q_OBJECT
private:
	struct ControllerType {
		QString nickName;
		QString fullName;
		QString iconURL;
	};
private:
	QList<ControllerType> controllerTypes;

public:
	explicit ControllerTypeModel(QObject *parent = nullptr);

	// list model
public:
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

public:
	void registerControllerType(const QString &nickName, const QString &fullName, const QString &iconURL);
	QString getFullName(const QModelIndex &index) const;
	QString getNickName(const QModelIndex &index) const;
	void clear();
};


#endif // CONTROLLERTYPEMODEL_HPP
