#ifndef HARDWAREWIZARD_HPP
#define HARDWAREWIZARD_HPP

#include <QWidget>






class HardwareTemplate
{
public:

	static const QString PROTOCOL_SERVOTOR32_2_1;
	static const QString INTERFACE_SERVOTOR32;
private:
	const QString mName;
	const QString mDescription;
	const QString mIconURL;
	const QString mInterface;
	const QString mProtocol;
	const quint32 mPoseSize;

public:

	HardwareTemplate(  const QString &name, const QString &description, const QString &iconURL,const QString &interface, const QString &protocol, const quint32 poseSize);

public:

	QString name() const;
	QString description() const;

	QString iconURL() const;
	QString interface()const;
	QString protocol()const;
	quint32 poseSize()const;

};



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////




namespace Ui
{
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

	void save();
public:
	explicit HardwareWizard(QWidget *parent = 0);
	virtual ~HardwareWizard();

public:

	void reset();

signals:

	void done();

private slots:
	void on_pushButtonOnward_clicked();

	void on_pushButtonBack_clicked();


};

#endif // HARDWAREWIZARD_HPP
