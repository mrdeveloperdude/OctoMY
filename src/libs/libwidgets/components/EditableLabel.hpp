#ifndef EDITABLELABEL_HPP
#define EDITABLELABEL_HPP


#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QIcon>

class EditableLabel : public QLabel
{
	Q_OBJECT
	
public:
	explicit EditableLabel(QWidget *parent = nullptr);
	void setText(const QString &text);
	QString text() const;
	
signals:
	void textChanged(const QString &newText);
	
protected:
	void mouseDoubleClickEvent(QMouseEvent *event) override;
	void resizeEvent(QResizeEvent *event) override;
	
private slots:
	void finishEditing();
	
private:
	QLineEdit *lineEdit;
	QLabel *iconLabel;
	QHBoxLayout *layout;
	bool isEditing;
};


#endif // EDITABLELABEL_HPP
