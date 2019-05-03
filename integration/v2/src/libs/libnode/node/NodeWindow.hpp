#ifndef NODEWINDOW_HPP
#define NODEWINDOW_HPP

#include "uptime/SharedPointerWrapper.hpp"

#include <QWidget>

class Node;

class NodeWindow : public QWidget, public QEnableSharedFromThis<NodeWindow>
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

	void focus();

	// Lifecycle stuff
protected:
	void nodeWindowRequestExit(const int returnValue=EXIT_SUCCESS);

	// Desktop stuff
protected:
	// Load position of window on screen
	void loadWindowGeometry();
	// Save position of window on screen
	void saveWindowGeometry();

	void updateWindowIcon();

	// QWidget interface
public:
	virtual void keyReleaseEvent(QKeyEvent *) Q_DECL_OVERRIDE;
	virtual	void closeEvent(QCloseEvent *) Q_DECL_OVERRIDE;
	virtual	void moveEvent(QMoveEvent *) Q_DECL_OVERRIDE;

	// Android stuff
protected:
	// Make a "normal" notification
	void notifyAndroid(QString);
	// Make a "toast" notification
	void toastAndroid(QString);

signals:
	void nodeWindowMoved(QSharedPointer <NodeWindow> window);
};

#endif
// NODEWINDOW_HPP
