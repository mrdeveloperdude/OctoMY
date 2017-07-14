#ifndef MTLOBEMANAGERWIDGET_HPP
#define MTLOBEMANAGERWIDGET_HPP

#include <QWidget>
#include <QList>

class MtlobeController;
namespace Ui
{
class MtlobeManagerWidget;
}

class MtlobeManagerWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::MtlobeManagerWidget *ui;
	QList<QWidget *> mWidgets;
	MtlobeController *mController;

public:
	explicit MtlobeManagerWidget(QWidget *parent = 0);
	~MtlobeManagerWidget();


private:
	void addWidget(QWidget &w);
	void removeWidget(QWidget &w);
public:

	void configure(MtlobeController *);

public slots:

	void mtlobeManagerChanged();




};

#endif // MTLOBEMANAGERWIDGET_HPP
