#ifndef POSEMAPPINGWIDGET_HPP
#define POSEMAPPINGWIDGET_HPP

#include <QWidget>


class QVBoxLayout;
class PoseMapping;
class QButtonGroup;
class QAbstractButton;

namespace Ui
{
class PoseMappingWidget;
}

class PoseMappingWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::PoseMappingWidget *ui;
	PoseMapping *mMapping;
	QButtonGroup *butGroupFrom;
	QButtonGroup *butGroupTo;

	QAbstractButton *fromButton;
	QAbstractButton *toButton;

public:
	explicit PoseMappingWidget(QWidget *parent = 0);
	virtual ~PoseMappingWidget();

public:

	void configure(PoseMapping &mapping);

	PoseMapping *mapping();
private:

	void addButtonPair(QVBoxLayout *hLayout, QString from,QString to, QButtonGroup *butGroupFrom, QButtonGroup *butGroupTo);

	void makeConnection();

	private slots:

	void onSpinValueChanged(int);
};

#endif // POSEMAPPINGWIDGET_HPP
