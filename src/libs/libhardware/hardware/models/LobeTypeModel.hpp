#ifndef LOBETYPEMODEL_HPP
#define LOBETYPEMODEL_HPP


#include <QAbstractListModel>
#include <QIcon>
#include <QString>

class LobeTypeModel : public QAbstractListModel
{
	Q_OBJECT
private:
	struct LobeType {
		QString nickName;
		QString fullName;
		QString iconURL;
	};
private:
	QList<LobeType> lobeTypes;
	
public:
	explicit LobeTypeModel(QObject *parent = nullptr);
	
	// list model
public:
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	
public:
	void registerLobeType(const QString &nickName, const QString &fullName, const QString &iconURL);
	QString getFullName(const QModelIndex &index) const;
	QString getNickName(const QModelIndex &index) const;
	void clear();
};




#endif // LOBETYPEMODEL_HPP
