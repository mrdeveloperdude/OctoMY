#ifndef REMOTEWINDOW_HPP
#define REMOTEWINDOW_HPP

#include "node/NodeWindow.hpp"

#include "app/log/LogDestination.hpp"

#include "uptime/SharedPointerWrapper.hpp"

class Remote;


namespace Ui
{
class RemoteWindow;
}

/*!
 * \brief The RemoteWindow class is the UI part of Remote. In it's current incarnation
 * it is required, but the long term goal is for remote to be able to run in
 * head-less mode, and in that mode the UI part will not be used.
 *
 * This is useful for when your remote is acting as a script host or in other
 * similar situations where the UI may not be needed.
 */

class RemoteWindow : public NodeWindow, public LogDestination
{
	Q_OBJECT

private:
	Ui::RemoteWindow *ui;

public:
	explicit RemoteWindow(QWidget *parent = nullptr);
	virtual ~RemoteWindow() override;

public:
	void configure() override;


	QSharedPointer<Remote> remote();


	// LogDestination interface
public:
	void appendLog(const QString& text) override;



};

#endif
// REMOTEWINDOW_HPP
