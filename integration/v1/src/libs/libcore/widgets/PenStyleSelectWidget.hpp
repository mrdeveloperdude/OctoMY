#ifndef PENSTYLESELECTWIDGET_HPP
#define PENSTYLESELECTWIDGET_HPP

#include <QWidget>
#include <QPen>

class PenStyleSelectWidget: public QWidget
{
	Q_OBJECT
private:

	QPen mBasePen;

public:
	explicit PenStyleSelectWidget(QWidget *parent = nullptr);
	virtual ~PenStyleSelectWidget();

private:

	void setupUi();

public:

	void setBasePen(QPen basePen);
	Qt::PenStyle penStyle();
	void setPenStyle(Qt::PenStyle style);


};

#endif // PENSTYLESELECTWIDGET_HPP
