#ifndef PENSTYLEDIALOG_HPP
#define PENSTYLEDIALOG_HPP

#include <QWidget>

class PenStyleDialog: public QWidget
{
	Q_OBJECT
private:
	static QDialog *dialog;
	static PenStyleDialog *penStyle;

public:
	explicit PenStyleDialog(QWidget *parent = nullptr);
	virtual ~PenStyleDialog();

private:

	void setupUi();
public:
	QPen pen();
public:
	static QPen getPen(QPen pen, QWidget *widget, QString title);
};

Q_DECLARE_METATYPE(PenStyleDialog*)

#endif // PENSTYLEDIALOG_HPP
