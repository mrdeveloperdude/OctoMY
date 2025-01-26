#ifndef SPREADSHEET_HPP
#define SPREADSHEET_HPP

#include <QWidget>

class QLineEdit;
class QModelIndex;
class QTableView;
class SpreadSheetModel;
class QUndoStack;

class SpreadSheet : public QWidget {
	Q_OBJECT
private:
	QLineEdit *mFormulaEditor;
	QTableView *mTableView;
	SpreadSheetModel *mModel;
	QUndoStack *mUndoStack;
	
public:
	explicit SpreadSheet(QWidget *parent = nullptr);

protected:
	void keyPressEvent(QKeyEvent *event) override;

private slots:
	void updateFormulaEditor();
	void updateCellsFromFormula();
	void copySelectionToClipboard();
	void pasteFromClipboard();
	void clearSelection();

};

#endif // SPREADSHEET_HPP
