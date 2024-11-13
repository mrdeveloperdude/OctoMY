#ifndef STANZALISTMODEL_HPP
#define STANZALISTMODEL_HPP


#include "uptime/SharedPointerWrapper.hpp"

#include <QDebug>
#include <QAbstractListModel>

class StanzaBook;

class StanzaListModel: public QAbstractListModel
{
	Q_OBJECT
private:
	QSharedPointer<StanzaBook> mStore;

public:
	explicit StanzaListModel(QSharedPointer<StanzaBook> stanzaBook);
	virtual ~StanzaListModel();

	// QAbstractListModel interface
public:
	int rowCount(const QModelIndex &) const override;
	int columnCount(const QModelIndex &) const override;
	QVariant data(const QModelIndex &index, int role) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

public:
	QString status();
	void invalidate();

public slots:
	void onStanzaChanged();

};

#endif
// STANZALISTMODEL_HPP
