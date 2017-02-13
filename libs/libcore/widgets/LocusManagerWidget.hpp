#ifndef LOCUSMANAGERWIDGET_HPP
#define LOCUSMANAGERWIDGET_HPP

#include <QWidget>
#include <QList>

namespace Ui
{
class LocusManagerWidget;
}

class LocusManagerWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::LocusManagerWidget *ui;
	QList<QWidget *> mWidgets;

public:
	explicit LocusManagerWidget(QWidget *parent = 0);
	~LocusManagerWidget();

public:

	void configure(quint32 num);

public slots:

	void onServoMoved(quint32 id, qreal val);
	void onServoLimped(quint32 id);

};

#endif // LOCUSMANAGERWIDGET_HPP
