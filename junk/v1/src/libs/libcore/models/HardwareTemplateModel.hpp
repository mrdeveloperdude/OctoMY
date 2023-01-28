#ifndef HARDWARETEMPLATEMODEL_HPP
#define HARDWARETEMPLATEMODEL_HPP



#include <QAbstractListModel>

class HardwareTemplate;

class HardwareTemplateModel : public QAbstractListModel
{
	Q_OBJECT

private:

	QVector<HardwareTemplate *> mTemplates;

public:
	explicit HardwareTemplateModel(QObject *parent = 0);

	// Header:
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	// Basic functionality:
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &) const;

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

signals:
	void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());
};

#endif // HARDWARETEMPLATEMODEL_HPP
