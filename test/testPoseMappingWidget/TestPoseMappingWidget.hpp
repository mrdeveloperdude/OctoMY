#ifndef TESTPOSEMAPPINGWIDGET_HPP
#define TESTPOSEMAPPINGWIDGET_HPP

#include "test/Common.hpp"


#include <QWidget>

class QImage;
class QPixmap;
class PoseMapping;
namespace Ui
{
class TestWindow;
}


class TestWindow : public QWidget
{
	Q_OBJECT
private:
	Ui::TestWindow *ui;

public:
	explicit TestWindow(QWidget *parent = nullptr, QString name="TestWindow");
	virtual ~TestWindow();

public:
	void configure(PoseMapping &pm);

private slots:
	void on_pushButtonClose_clicked();

};


class TestPoseMappingWidget:public QObject
{
	Q_OBJECT

private slots:
	void testBasics();
	void testWidget();

};


#endif
// TESTPOSEMAPPINGWIDGET_HPP
