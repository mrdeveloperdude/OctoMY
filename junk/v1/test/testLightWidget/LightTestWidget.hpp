#ifndef LIGHTTESTWIDGET_HPP
#define LIGHTTESTWIDGET_HPP

#include <QWidget>
#include <QTimer>

class LightWidget;
namespace Ui
{
class LightTestWidget;
}

class LightTestWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::LightTestWidget *ui;
	QColor mColor;
	QList<LightWidget *> mLights;
	QTimer mTimer;

public:
	explicit LightTestWidget(QWidget *parent = 0);
	~LightTestWidget();
private:
	void addLight(int x, int y,int w=0, int h=0);
private slots:
	void on_pushButtonSelectColor_clicked();


	void on_pushButtonLightOn_clicked();
	void on_pushButtonLightOff_clicked();
	void on_pushButtonToggleLight_clicked();
	void on_pushButtonBlink_toggled(bool checked);
};

#endif // LIGHTTESTWIDGET_HPP
