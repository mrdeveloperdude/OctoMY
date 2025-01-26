#ifndef SPREADSHEETMODEL_HPP
#define SPREADSHEETMODEL_HPP

#include "CellData.hpp"

#include <QAbstractTableModel>


class SpreadSheetModel : public QAbstractTableModel {
	Q_OBJECT
	
private:
	int mRows;
	int mColumns;
	QVector<CellData> mCellData; // Holds cell data for the spreadsheet
	
public:
	explicit SpreadSheetModel(int rows = 100, int columns = 26, QObject *parent = nullptr);

	// Required overrides
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;
	
	// Utility methods
	void setCellExpression(int row, int column, const QString &expression);
	QString cellExpression(int row, int column) const;
	qreal cellValue(int row, int column) const;
};


#endif // SPREADSHEETMODEL_HPP
