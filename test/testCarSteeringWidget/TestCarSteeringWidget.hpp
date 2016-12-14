#ifndef TESTCARSTEERINGWIDGET_HPP
#define TESTCARSTEERINGWIDGET_HPP

#include <QTest>



#include <QWidget>

class QImage;
class QPixmap;
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
	explicit TestWindow(QWidget *parent = 0, QString name="TestWindow");
	virtual ~TestWindow();

public:

private slots:
	void on_pushButtonClose_clicked();


};

class TestCarSteeringWidget:public QObject
{
	Q_OBJECT

private slots:
	void test();

};


#endif // TESTCARSTEERINGWIDGET_HPP
