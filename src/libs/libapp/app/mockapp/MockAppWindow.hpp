#ifndef MOCKAPPWINDOW_HPP
#define MOCKAPPWINDOW_HPP

#include "node/NodeWindow.hpp"

#include "app/log/LogDestination.hpp"

#include "uptime/SharedPointerWrapper.hpp"

class MockApp;

namespace Ui
{
class MockAppWindow;
}

class MockAppWindow: public NodeWindow, public LogDestination
{
	Q_OBJECT

private:
	Ui::MockAppWindow *ui;
	
public:
	explicit MockAppWindow(QWidget *parent = nullptr);
	virtual ~MockAppWindow() Q_DECL_OVERRIDE;
	
	// NodeWindow interface
public:
	void configure() Q_DECL_OVERRIDE;

public:
	QSharedPointer<MockApp> mockApp();

	// LogDestination interface
public:
	void appendLog(const QString& text) Q_DECL_OVERRIDE;


};

#endif // MOCKAPPWINDOW_HPP
