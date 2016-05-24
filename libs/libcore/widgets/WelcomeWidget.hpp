#ifndef WELCOMEWIDGET_HPP
#define WELCOMEWIDGET_HPP

#include <QWidget>

namespace Ui {
	class WelcomeWidget;
}

class WelcomeWidget : public QWidget
{
		Q_OBJECT

	public:
		explicit WelcomeWidget(QWidget *parent = 0);
		~WelcomeWidget();

	private slots:
		void on_pushButtonYoutubeTutorial_clicked();

		void on_pushButtonInitialize_clicked();

	private:
		Ui::WelcomeWidget *ui;

	signals:
		void initialized();

};

#endif // WELCOMEWIDGET_HPP
