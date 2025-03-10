#include "EditableLabel.hpp"

#include <QMouseEvent>
#include <QStyle>
#include <QPixmap>

EditableLabel::EditableLabel(QWidget *parent)
	: QLabel(parent), isEditing(false)
{
	// Initialize QLineEdit for editing mode
	lineEdit = new QLineEdit(this);
	lineEdit->hide();
	
	// Initialize QLabel for edit icon and set the icon
	iconLabel = new QLabel(this);
	iconLabel->setPixmap(QIcon(":/icons/app/edit.svg").pixmap(16, 16));  // Set the edit icon size
	
	// Set up layout to hold text and icon
	layout = new QHBoxLayout(this);
	layout->setContentsMargins(3, 3, 3, 3);
	layout->addWidget(iconLabel);
	layout->addWidget(lineEdit);
	layout->setAlignment(Qt::AlignRight);
	
	// Set initial text
	setText("Editable Label");
	
	// Connect editing finished signal to finish editing slot
	connect(lineEdit, &QLineEdit::editingFinished, this, &EditableLabel::finishEditing);
}

void EditableLabel::setText(const QString &text)
{
	QLabel::setText(text);  // Set text directly on QLabel
	lineEdit->setText(text);  // Sync line edit text
}

QString EditableLabel::text() const
{
	return lineEdit->text();  // Return text from line edit
}

void EditableLabel::mouseDoubleClickEvent(QMouseEvent *event)
{
	Q_UNUSED(event);
	isEditing = true;
	lineEdit->setText(text());
	lineEdit->setVisible(true);
	iconLabel->setVisible(false);
	lineEdit->setAlignment(this->alignment());
	lineEdit->setFocus();
	lineEdit->selectAll();
}

void EditableLabel::finishEditing()
{
	isEditing = false;
	lineEdit->setVisible(false);
	iconLabel->setVisible(true);
	const auto text = lineEdit->text();
	setText(text);
	emit textChanged(text);
}

void EditableLabel::resizeEvent(QResizeEvent *event)
{
	QLabel::resizeEvent(event);
	lineEdit->setGeometry(rect());
}
