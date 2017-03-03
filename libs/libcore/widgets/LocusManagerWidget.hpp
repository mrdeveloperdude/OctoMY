#ifndef LOCUSMANAGERWIDGET_HPP
#define LOCUSMANAGERWIDGET_HPP

#include <QWidget>
#include <QList>

class LocusController;
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
	LocusController *mController;

public:
	explicit LocusManagerWidget(QWidget *parent = 0);
	~LocusManagerWidget();


private:
	void addWidget(QWidget &w);
	void removeWidget(QWidget &w);
public:

	void configure(LocusController *);

public slots:

	void locusManagerChanged();




};

#endif // LOCUSMANAGERWIDGET_HPP
