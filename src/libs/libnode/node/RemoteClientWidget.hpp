#ifndef REMOTECLIENTWIDGET_HPP
#define REMOTECLIENTWIDGET_HPP


#include "comms/CommsSession.hpp"

#include "widgets/TryToggle.hpp"
#include "node/Node.hpp"
#include "node/RemoteCourierSet.hpp"


#include <QSharedPointer>
#include <QWidget>
#include <QTimer>


class WaitingSpinnerWidget;
class SensorsCourier;
class AgentStateCourier;
class BlobCourier;
class ISyncParameter;

namespace Ui
{
class ClientWidget;
}

/**
 * @brief The ClientWidget class is the UI for one single Agent as seen on the remote
 */

class RemoteClientWidget : public QWidget
{
	Q_OBJECT

private:
	Ui::ClientWidget *ui;

	WaitingSpinnerWidget *mSpinner;
	QSharedPointer <RemoteClient> mRemoteClient;



public:
	explicit RemoteClientWidget(QSharedPointer<RemoteClient> client, QWidget *parent=nullptr);
	virtual ~RemoteClientWidget();

private:
	bool eventFilter(QObject *object, QEvent *event);

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

	CommsChannel *comms();
	QSharedPointer<Associate> nodeAssociate() const;
	QSharedPointer<Node> controller();

	void updateControlLevel(int level);


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

#endif // REMOTECLIENTWIDGET_HPP
