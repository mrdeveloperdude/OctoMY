#ifndef REMOTECLIENTWIDGET_HPP
#define REMOTECLIENTWIDGET_HPP

#include "client/ClientWidget.hpp"
#include "comms/couriers/sets/RemoteCourierSet.hpp"
#include "components/TryToggleState.hpp"
#include "node/Node.hpp"

#include <QWidget>
#include <QTimer>


class WaitingSpinnerWidget;
class SensorsCourier;
class AgentStateCourier;
class BlobCourier;
class ISyncParameter;

namespace Ui
{
class RemoteClientWidget;
}

/**
 * @brief The ClientWidget class is the UI for one single Agent as seen on the remote
 */

class RemoteClientWidget : public ClientWidget
{
	Q_OBJECT

private:
	Ui::RemoteClientWidget *ui;
	WaitingSpinnerWidget *mSpinner;
	QSharedPointer <RemoteClient> mRemoteClient;

public:
	explicit RemoteClientWidget(QSharedPointer<RemoteClient> client, QWidget *parent=nullptr);
	virtual ~RemoteClientWidget();

private:
	bool eventFilter(QObject *object, QEvent *event) override;

	// Spinner
	void prepareSpinner();
	void prepareSteering();
	void setSpinnerActive(bool active);
	void init();
	void updateOnlineStatus();
	/*
	bool courierRegistration();
	void setCourierRegistration(bool reg);
	*/
	bool setSetting(QString key, bool val);

public:
	bool needConnection();

public:
	QSharedPointer<Comms> comms();
	QSharedPointer<Associate> nodeAssociate();
	QSharedPointer<Node> controller();

	// ClientWidget interface
public:
	void updateControlLevel(int level) override;

	// Internal custom UI slots
private slots:
	void onConnectButtonStateChanged(const TryToggleState, const TryToggleState);
	void onSteeringChanged(qreal throttle, qreal steeringAngle);

	// Internal UI slots
public slots:
	void on_checkBoxShowEyes_toggled(bool checked);
	void on_checkBoxShowStats_toggled(bool checked);
	void on_checkBoxShowLog_toggled(bool checked);
	void on_checkBoxShowOnlineButton_toggled(bool checked);
	void on_widgetPanic_toggled(bool checked);

};

#endif
// REMOTECLIENTWIDGET_HPP
