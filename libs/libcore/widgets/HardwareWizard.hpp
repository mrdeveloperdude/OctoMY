#ifndef HARDWAREWIZARD_HPP
#define HARDWAREWIZARD_HPP

#include <QWidget>

namespace Ui {
	class HardwareWizard;
}


class SerialDeviceListModel;
class HardwareWizard : public QWidget
{
		Q_OBJECT

	private:
		Ui::HardwareWizard *ui;
		SerialDeviceListModel *mSerialDevices;

		void moveTo(int next);
	public:
		explicit HardwareWizard(QWidget *parent = 0);
		virtual ~HardwareWizard();

	public:

		void reset();

	private slots:
		void on_pushButtonOnward_clicked();

		void on_pushButtonBack_clicked();


};

#endif // HARDWAREWIZARD_HPP
