#ifndef CONTROLPAIRINGWIDGET_HPP
#define CONTROLPAIRINGWIDGET_HPP

#include <QWidget>

namespace Ui {
	class ControlPairingWidget;
}

class ControlPairingWidget : public QWidget
{
		Q_OBJECT

	private:
		Ui::ControlPairingWidget *ui;


	public:
		explicit ControlPairingWidget(QWidget *parent = 0);
		~ControlPairingWidget();


	private:
		void save();
	public:
		void reset();

	private slots:
		void on_pushButtonDone_clicked();

		void on_pushButtonYoutubeTutorial_clicked();

		void on_pushButtonSkip_clicked();

		void on_pushButtonMaybeOnward_clicked();

	signals:
		void done();
};

#endif // CONTROLPAIRINGWIDGET_HPP
