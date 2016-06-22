#ifndef HUDWIDGET_HPP
#define HUDWIDGET_HPP

#include <QWidget>
#include <QTimer>


namespace Ui {
	class HUDWidget;
}

class HUDWidget : public QWidget
{
		Q_OBJECT
	private:
		Ui::HUDWidget *ui;
		QTimer gaugeTimer;
		quint64 startTime;

	public:
		explicit HUDWidget(QWidget *parent = 0);
		virtual ~HUDWidget();

	protected:

		virtual void showEvent(QShowEvent *);
		virtual void hideEvent(QHideEvent *);

	private slots:
		void onGaugeTimer();
};

#endif // HUDWIDGET_HPP
