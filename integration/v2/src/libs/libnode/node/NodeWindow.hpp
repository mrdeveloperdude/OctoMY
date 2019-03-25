#ifndef NODEWINDOW_HPP
#define NODEWINDOW_HPP

#include "uptime/SharedPointerWrapper.hpp"

#include <QWidget>

class Node;

class NodeWindow : public QWidget
{
	Q_OBJECT
private:
	QSharedPointer<Node> mNode;

public:
	explicit NodeWindow(QWidget *parent = nullptr);
	virtual ~NodeWindow() Q_DECL_OVERRIDE;

public:
	void nodeWindowConfigure(QSharedPointer<Node> node);
	virtual void configure()=0;

	QSharedPointer<Node> node();

	// Lifecycle stuff
protected:
	void nodeWindowRequestExit(const int returnValue=EXIT_SUCCESS);

	// Desktop stuff
protected:
	// Load position of window on screen
	void loadWindowGeometry();
	// Save position of window on screen
	void saveWindowGeometry();


	// Android stuff
protected:
	// Make a "normal" notification
	void notifyAndroid(QString);
	// Make a "toast" notification
	void toastAndroid(QString);
};

#endif
// NODEWINDOW_HPP
