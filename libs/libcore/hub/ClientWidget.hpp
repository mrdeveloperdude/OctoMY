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
	SensorsCourier *mSensorsCourier;


public:
	explicit ClientWidget(QSharedPointer<Node> controller, QSharedPointer<NodeAssociate> nodeAssoc, QWidget *parent=nullptr);
	~ClientWidget();

private:
	bool eventFilter(QObject *object, QEvent *event);

	// Spinner
	void prepareSpinner();
	void setSpinnerActive(bool active);

	void init();

	CommsChannel *comms();

	void courierRegistration(bool reg);

public:

	void updateControlLevel(int level);

	// Internal slots
public slots:
	void onSummaryTimer();
	void appendLog(const QString& text);
	void appendSpeechHistory(const QString& text);




	// Internal UI slots
private slots:

	void onConnectionStateChanged(const TryToggleState, const TryToggleState);
	void on_pushButtonSay_clicked();

	void on_checkBoxShowEyes_toggled(bool checked);
	void on_checkBoxShowStats_toggled(bool checked);
	void on_checkBoxShowLog_toggled(bool checked);
	void on_checkBoxShowOnlineButton_toggled(bool checked);
};

#endif // CLIENTWINDOW_HPP
