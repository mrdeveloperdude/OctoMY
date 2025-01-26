#include "EditMultipleCommand.hpp"

EditMultipleCommand::EditMultipleCommand(SpreadSheetModel *model, const QVector<QModelIndex> &cells, const QString &newExpression, QUndoCommand *parent)
	: QUndoCommand(parent)
	, mModel(model)
	, mCells(cells)
{
	// Store old expressions and set the new uniform expression for all cells
	for (const auto &cell : cells) {
		QString oldExpression = model->data(cell, Qt::EditRole).toString();
		mExpressions.append(qMakePair(oldExpression, newExpression));
	}
}

EditMultipleCommand::EditMultipleCommand(SpreadSheetModel *model, const QVector<QModelIndex> &cells, const QVector<QString> &newExpressions, QUndoCommand *parent)
	: QUndoCommand(parent)
	, mModel(model)
	, mCells(cells)
{
	// Store old expressions and the provided new expressions for all cells
	for (int i = 0; i < cells.size(); ++i) {
		QString oldExpression = model->data(cells[i], Qt::EditRole).toString();
		mExpressions.append(qMakePair(oldExpression, newExpressions[i]));
	}
}

void EditMultipleCommand::undo() {
	// Restore old expressions for all cells
	for (int i = 0; i < mCells.size(); ++i) {
		mModel->setData(mCells[i], mExpressions[i].first, Qt::EditRole);
	}
}

void EditMultipleCommand::redo() {
	// Apply new expressions for all cells
	for (int i = 0; i < mCells.size(); ++i) {
		mModel->setData(mCells[i], mExpressions[i].second, Qt::EditRole);
	}
}
