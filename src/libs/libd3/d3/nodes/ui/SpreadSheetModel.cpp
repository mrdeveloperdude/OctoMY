#include "SpreadSheetModel.hpp"

#include <QRegularExpression>

SpreadSheetModel::SpreadSheetModel(int rows, int columns, QObject *parent)
	: QAbstractTableModel(parent), mRows(rows), mColumns(columns), mCellData(rows * columns) {}

int SpreadSheetModel::rowCount(const QModelIndex &parent) const {
	Q_UNUSED(parent);
	return mRows;
}

int SpreadSheetModel::columnCount(const QModelIndex &parent) const {
	Q_UNUSED(parent);
	return mColumns;
}


QVariant SpreadSheetModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if (role != Qt::DisplayRole) return QVariant();
	
	if (orientation == Qt::Horizontal) {
		// Convert column number to letters (A, B, ..., Z, AA, AB, ...)
		QString result;
		int col = section;
		do {
			result.prepend(QChar('A' + (col % 26)));
			col = col / 26 - 1;
		} while (col >= 0);
		return result;
	} else if (orientation == Qt::Vertical) {
		return QString::number(section + 1); // Rows are numbered 1, 2, ...
	}
	
	return QVariant();
}

Qt::ItemFlags SpreadSheetModel::flags(const QModelIndex &index) const {
	if (!index.isValid()) return Qt::NoItemFlags;
	return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

void SpreadSheetModel::setCellExpression(int row, int column, const QString &expression) {
	if (row < 0 || row >= mRows || column < 0 || column >= mColumns) return;
	
	auto &cell = mCellData[row * mColumns + column];
	cell.expression = expression;
	cell.update();
	
	const QModelIndex index = createIndex(row, column);
	emit dataChanged(index, index);
}

QString SpreadSheetModel::cellExpression(int row, int column) const {
	if (row < 0 || row >= mRows || column < 0 || column >= mColumns) return QString();
	return mCellData[row * mColumns + column].expression;
}

qreal SpreadSheetModel::cellValue(int row, int column) const {
	if (row < 0 || row >= mRows || column < 0 || column >= mColumns) return 0.0;
	return mCellData[row * mColumns + column].value;
}


QVariant SpreadSheetModel::data(const QModelIndex &index, int role) const {
	if (!index.isValid()) return QVariant();
	
	const auto &cell = mCellData[index.row() * mColumns + index.column()];
	
	if (role == Qt::DisplayRole) {
		if (cell.expression.isEmpty()) {
			return QVariant(); // Empty cell
		} else if (cell.expression.startsWith("=")) {
			return cell.value; // Show value for expressions
		} else {
			return cell.expression; // Show literal text
		}
	} else if (role == Qt::EditRole) {
		return cell.expression; // Allow editing of expression
	}
	
	return QVariant();
}

bool SpreadSheetModel::setData(const QModelIndex &index, const QVariant &value, int role) {
	if (!index.isValid() || role != Qt::EditRole) return false;
	
	auto &cell = mCellData[index.row() * mColumns + index.column()];
	cell.expression = value.toString();
	cell.update();
	
	emit dataChanged(index, index);
	return true;
}
