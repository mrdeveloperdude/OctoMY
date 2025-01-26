#include "CellDelegate.hpp"

#include <QKeyEvent>
#include <QEvent>

CellDelegate::CellDelegate(QObject *parent)
	: QStyledItemDelegate(parent) {}

QWidget *CellDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
	Q_UNUSED(option);
	Q_UNUSED(index);
	// Create a QLineEdit for editing
	auto *editor = new QLineEdit(parent);
	editor->installEventFilter(const_cast<CellDelegate *>(this)); // Filter events for the editor
	return editor;
}

void CellDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
	auto *lineEdit = qobject_cast<QLineEdit *>(editor);
	if (!lineEdit) return;
	
	// Set the cell's expression to the editor
	lineEdit->setText(index.model()->data(index, Qt::EditRole).toString());
}

void CellDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
	auto *lineEdit = qobject_cast<QLineEdit *>(editor);
	if (!lineEdit) return;
	
	// Save the editor's text back to the model
	model->setData(index, lineEdit->text(), Qt::EditRole);
}

void CellDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const {
	Q_UNUSED(index);
	
	// Ensure the editor matches the cell geometry
	editor->setGeometry(option.rect);
}

bool CellDelegate::eventFilter(QObject *object, QEvent *event) {
	auto *lineEdit = qobject_cast<QLineEdit *>(object);
	if (!lineEdit) {
		return QStyledItemDelegate::eventFilter(object, event);
	}
	
	// Handle key events specifically
	if (event->type() == QEvent::KeyPress) {
		auto *keyEvent = static_cast<QKeyEvent *>(event);
		if (keyEvent->key() == Qt::Key_Escape) {
			// Cancel editing
			lineEdit->clearFocus();
			return true;
		}
	}
	
	return QStyledItemDelegate::eventFilter(object, event);
}
