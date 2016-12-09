#ifndef CLIENTWINDOW_HPP
#define CLIENTWINDOW_HPP

#include <QWidget>
#include <QTimer>

#include "comms/Client.hpp"

#include "widgets/TryToggle.hpp"
#include "basic/Node.hpp"


#include <QSharedPointer>

class WaitingSpinnerWidget;
class SensorsCourier;
class AgentStateCourier;
class BlobCourier;
class ISyncParameter;

namespace Ui
{
class ClientWidget;
}

class ClientWidget : public QWidget
{
	Q_OBJECT

private:
	Ui::ClientWidget *ui;
	QTimer updateTimer;
	QSharedPointer<Node> mController;
	QSharedPointer<NodeAssociate> mNodeAssoc;
	WaitingSpinnerWidget *mSpinner;
	AgentStateCourier *mAgentStateCourier;
	SensorsCourier *mSensorsCourier;
	BlobCourier *mBlobCourier;

public:
	explicit ClientWidget(QSharedPointer<Node> controller, QSharedPointer<NodeAssociate> nodeAssoc, QWidget *parent=nullptr);
	virtual ~ClientWidget();

private:
	bool eventFilter(QObject *object, QEvent *event);

	// Spinner
	void prepareSpinner();
	void setSpinnerActive(bool active);

	void init();

	void updateOnlineStatus();

	bool courierRegistration();
	void setCourierRegistration(bool reg);



public:
	CommsChannel *comms();
	QSharedPointer<NodeAssociate> nodeAssoc() const;

	void updateControlLevel(int level);

	// Agent State Courier slots
public slots:
	void onValueChanged(ISyncParameter *);

	// CommsChannel slots
private slots:
	void onCommsError(QString);
	void onCommsClientAdded(Client *);
	void onCommsConnectionStatusChanged(bool);



	// Internal slots
public slots:
	void onUpdateTimer();
	void appendLog(const QString& text);


	// Internal custom UI slots
private slots:

	void onConnectButtonStateChanged(const TryToggleState, const TryToggleState);

	// Internal UI slots


public slots:

	void on_checkBoxShowEyes_toggled(bool checked);
	void on_checkBoxShowStats_toggled(bool checked);
	void on_checkBoxShowLog_toggled(bool checked);
	void on_checkBoxShowOnlineButton_toggled(bool checked);
	void on_pushButtonPanicc_toggled(bool checked);


};

#endif // CLIENTWINDOW_HPP
