#ifndef HAARCASCADEMODEL_HPP
#define HAARCASCADEMODEL_HPP

#include <QAbstractListModel>
#include <QImage>
#include "uptime/SharedPointerWrapper.hpp"
#include "HaarCascade.hpp"

class HaarCascadeModel : public QAbstractListModel {
	Q_OBJECT
	
private:
	QList<QSharedPointer<HaarCascade>> m_cascades;
	
public:
	enum Role {
		NameRole = Qt::UserRole + 1,
		ImageRole,
		FilenameRole
	};
	HaarCascadeModel(QObject *parent = nullptr);

	// QAbstractListModel interface
public:
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;	
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	void addCascade(QSharedPointer<HaarCascade> cascade);
	QSharedPointer<HaarCascade> cascadeAt(int index) const;

};


#endif // HAARCASCADEMODEL_HPP

