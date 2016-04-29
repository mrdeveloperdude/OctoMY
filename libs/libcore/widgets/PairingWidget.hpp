#ifndef PAIRINGWIDGET_HPP
#define PAIRINGWIDGET_HPP

#include <QWidget>
#include <QModelIndex>

namespace Ui {
	class PairingWidget;
}

class PairingWidget : public QWidget
{
		Q_OBJECT

	public:
		explicit PairingWidget(QWidget *parent = 0);
		~PairingWidget();


	private slots:
		void on_checkBoxSecurity_toggled(bool checked);
		void on_pushButtonBack_clicked();
		void on_pushButtonPairLAN_clicked();
		void on_pushButtonPairCamera_clicked();
		void on_pushButtonBack_2_clicked();
		void on_pushButtonPairZOO_clicked();
		void on_pushButtonPairBluetooth_clicked();
		void on_pushButtonPairNFC_clicked();
		void on_pushButtonBack_3_clicked();
		void on_pushButtonBack_4_clicked();
		void on_listWidgetScanned_doubleClicked(const QModelIndex &index);
		void on_listWidgetPaired_doubleClicked(const QModelIndex &index);

	private:
		Ui::PairingWidget *ui;
};

#endif // PAIRINGWIDGET_HPP
