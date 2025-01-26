#ifndef EDITMULTIPLECOMMAND_HPP
#define EDITMULTIPLECOMMAND_HPP

#include <QUndoCommand>
#include <QModelIndex>
#include <QVector>
#include <QPair>
#include <QString>
#include "SpreadSheetModel.hpp"

class EditMultipleCommand : public QUndoCommand {
private:
	SpreadSheetModel *mModel;
	QVector<QModelIndex> mCells; // Target cells
	QVector<QPair<QString, QString>> mExpressions; // Old and new expressions for each cell
	
public:
	// Constructor for uniform updates (e.g., single value for all cells)
	EditMultipleCommand(SpreadSheetModel *model, const QVector<QModelIndex> &cells, const QString &newExpression, QUndoCommand *parent = nullptr);
	
	// Constructor for individual updates (e.g., paste operation with unique values per cell)
	EditMultipleCommand(SpreadSheetModel *model, const QVector<QModelIndex> &cells, const QVector<QString> &newExpressions, QUndoCommand *parent = nullptr);
	
	void undo() override;
	void redo() override;
};

#endif // EDITMULTIPLECOMMAND_HPP
