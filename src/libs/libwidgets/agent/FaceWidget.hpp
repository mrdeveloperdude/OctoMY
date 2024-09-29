/* This file is distributed under the license as specified in the LICENSE file
 * that accompanied it. If there was no LICENSE file accompanying this source,
 * it is not to be modified, redistributed or used without prior written
 * permission from original author and owner "Lennart Rolland".
 */

#ifndef FACEWIDGET_HPP
#define FACEWIDGET_HPP

#include "app/log/LogDestination.hpp"
#include "components/TryToggleState.hpp"
#include "security/PortableID.hpp"
#include "uptime/ConfigureHelper.hpp"
#include "components/navigation/Activity.hpp"

#include <QWidget>

class CommsSession;
class Agent;
class Settings;
class ISyncParameter;

namespace Ui
{
class FaceWidget;
}

class FaceWidget : public Activity, public LogDestination
{
	Q_OBJECT
private:
	Ui::FaceWidget *ui;
	PortableID mLastPID;
	QSharedPointer<Agent> mAgent;
	ConfigureHelper mConfigureHelper;

public:
	explicit FaceWidget(QWidget *parent = nullptr);
	~FaceWidget();

protected:
	QSharedPointer<Agent> agent();
	QSharedPointer<Settings> settings();

	// LogDestination interface
public:
	void appendLog(const QString& text) override;

public:
	void updateEyeColor();
	void updateVisibility();
	
	void setAgent(QSharedPointer<Agent>);
	void setHookSignals(QObject &ob, bool hook);
	void setPanic(bool panic);

signals:
	void connectionStateChanged(const TryToggleState last, const TryToggleState current);
	void colorChanged(QColor);

public slots:
	void onSyncParameterChanged(ISyncParameter *);

private slots:
	void onColorClicked();
	void onPanicToggled(bool checked);
	void onSplitterMoved(int pos, int index);

	// CommsChannel slots (via setHookCommsSignals in AgentWindow)
public slots:
	// An error occurred in comms
	void onCommsError(QString message);
	// A new comms session was added
	void onCommsClientAdded(CommsSession *c);
	// The connection state changed for comms channel
	void onCommsConnectionStatusChanged(const bool isConnected, const bool needsConnection);

};

#endif // FACEWIDGET_HPP
