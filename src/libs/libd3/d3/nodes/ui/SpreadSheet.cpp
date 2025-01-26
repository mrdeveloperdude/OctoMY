#include "SpreadSheet.hpp"

#include "SpreadSheetModel.hpp"
#include "d3/nodes/ui/EditMultipleCommand.hpp"
#include "CellDelegate.hpp"

#include <QAbstractTableModel>
#include <QApplication>
#include <QClipboard>
#include <QHeaderView>
#include <QHeaderView>
#include <QKeyEvent>
#include <QLineEdit>
#include <QTableView>
#include <QUndoStack>
#include <QVBoxLayout>
#include <QVBoxLayout>


SpreadSheet::SpreadSheet(QWidget *parent)
	: QWidget(parent)
	, mFormulaEditor(new QLineEdit(this))
	, mTableView(new QTableView(this))
	, mModel(new SpreadSheetModel(100, 26, this))
	, mUndoStack(new QUndoStack(this)) {
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(mFormulaEditor);
	layout->addWidget(mTableView);
	
	mTableView->setModel(mModel);
	mTableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
	mTableView->setSelectionBehavior(QAbstractItemView::SelectItems);
	
	auto delegate = new CellDelegate(this);
	mTableView->setItemDelegate(delegate);
	
	connect(mTableView->selectionModel(), &QItemSelectionModel::selectionChanged,
			this, &SpreadSheet::updateFormulaEditor);
	connect(mFormulaEditor, &QLineEdit::returnPressed,
			this, &SpreadSheet::updateCellsFromFormula);
	
	// Update formula editor when editing is completed
	connect(mTableView->itemDelegate(), &QAbstractItemDelegate::closeEditor,
			this, &SpreadSheet::updateFormulaEditor);
}


void SpreadSheet::keyPressEvent(QKeyEvent *event) {
	if (event->matches(QKeySequence::Copy)) {
		copySelectionToClipboard();
	} else if (event->matches(QKeySequence::Paste)) {
		pasteFromClipboard();
	} else if (event->key() == Qt::Key_Delete) {
		clearSelection();
	} else if (event->matches(QKeySequence::Undo)) {
		mUndoStack->undo();
	} else if (event->matches(QKeySequence::Redo)) {
		mUndoStack->redo();
	} else {
		QWidget::keyPressEvent(event);
	}
}


void SpreadSheet::updateFormulaEditor() {
	auto selection = mTableView->selectionModel()->selectedIndexes();
	if (selection.size() == 1) {
		const auto index = selection.first();
		mFormulaEditor->setText(mModel->data(index, Qt::EditRole).toString());
	} else {
		mFormulaEditor->clear(); // Clear for multi-cell selection
	}
}


void SpreadSheet::updateCellsFromFormula() {
	const QString expression = mFormulaEditor->text();
	auto selection = mTableView->selectionModel()->selectedIndexes();
	if (selection.isEmpty()) {
		return;
	}
	mUndoStack->push(new EditMultipleCommand(mModel, selection, expression));
}



void SpreadSheet::copySelectionToClipboard() {
	auto selection = mTableView->selectionModel()->selectedIndexes();
	if (selection.isEmpty()) {
		return;
	}
	
	// Sort the indexes row-major order
	std::sort(selection.begin(), selection.end(),
			  [](const QModelIndex &a, const QModelIndex &b) {
				  return a.row() < b.row() || (a.row() == b.row() && a.column() < b.column());
			  });
	
	QString clipboardText;
	int currentRow = selection.first().row();
	
	for (const auto &index : selection) {
		if (index.row() != currentRow) {
			clipboardText += '\n';
			currentRow = index.row();
		} else if (!clipboardText.isEmpty()) {
			clipboardText += '\t';
		}
		clipboardText += mModel->data(index, Qt::EditRole).toString();
	}
	
	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setText(clipboardText);
}

void SpreadSheet::pasteFromClipboard() {
	QClipboard *clipboard = QApplication::clipboard();
	const QString clipboardText = clipboard->text();
	
	auto selection = mTableView->selectionModel()->selectedIndexes();
	if (selection.isEmpty()) {
		return;
	}
	
	const auto rows = clipboardText.split('\n');
	int startRow = selection.first().row();
	int startCol = selection.first().column();
	
	QVector<QModelIndex> targetCells;
	QVector<QString> newExpressions;
	
	for (int i = 0; i < rows.size(); ++i) {
		const auto cols = rows[i].split('\t');
		for (int j = 0; j < cols.size(); ++j) {
			QModelIndex index = mModel->index(startRow + i, startCol + j);
			if (index.isValid()) {
				targetCells.append(index);
				newExpressions.append(cols[j]);
			}
		}
	}
	
	if (!targetCells.isEmpty()) {
		mUndoStack->push(new EditMultipleCommand(mModel, targetCells, newExpressions));
	}
}


void SpreadSheet::clearSelection() {
	auto selection = mTableView->selectionModel()->selectedIndexes();
	if (selection.isEmpty()) {
		return;
	}
	QVector<QString> emptyExpressions(selection.size(), "");
	mUndoStack->push(new EditMultipleCommand(mModel, selection, emptyExpressions));
}

