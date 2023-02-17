#ifndef REMOTECLIENTWIDGET_HPP
#define REMOTECLIENTWIDGET_HPP


#include "address/Associate.hpp"
#include "client/ClientWidget.hpp"
#include "node/Node.hpp"
#include "components/TryToggleState.hpp"

#include <QWidget>
#include <QTimer>


class WaitingSpinnerWidget;
class SensorsCourier;
class AgentStateCourier;
class BlobCourier;
class ISyncParameter;
class AgentClient;

namespace Ui
{
class AgentClientWidget;
}

/**
 * @brief The ClientWidget class is the UI for one single Agent as seen on the remote
 */

class AgentClientWidget : public ClientWidget
{
	Q_OBJECT

private:
	Ui::AgentClientWidget *ui;

	WaitingSpinnerWidget *mSpinner;
	QSharedPointer <AgentClient> mAgentClient;

public:
	explicit AgentClientWidget(QSharedPointer<AgentClient> client, QWidget *parent=nullptr);
	virtual ~AgentClientWidget() Q_DECL_OVERRIDE;

private:
	bool eventFilter(QObject *object, QEvent *event) Q_DECL_OVERRIDE;

	// Spinner
	void prepareSpinner();
	void prepareSteering();
	void setSpinnerActive(bool active);

	void init();

	void updateOnlineStatus();

	QString id();

	bool setSetting(QString key, bool val);

public:

	QSharedPointer<CommsChannel> comms();
	QSharedPointer<Associate> nodeAssociate() const;
	QSharedPointer<Node> controller();

	// ClientWidget interface
public:
	void updateControlLevel(int level) Q_DECL_OVERRIDE;


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
