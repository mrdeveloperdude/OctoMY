#ifndef NODEWINDOW_HPP
#define NODEWINDOW_HPP

#include "uptime/SharedPointerWrapper.hpp"

#include <QWidget>
#include <QSvgRenderer>

class ActivityStack;
class Node;
class SplashScreen;

class NodeWindow : public QWidget, public QEnableSharedFromThis<NodeWindow>
{
	Q_OBJECT
private:
	QSharedPointer<Node> mNode;
protected:
	QSvgRenderer mWaterMark;
	QPixmap mWatermarkCache;
	QSize mWatermarkLastSize;
	SplashScreen *mSplash{nullptr};
	ActivityStack *mActivityStack{nullptr};
	bool mDebug{true};
	
public:
	explicit NodeWindow(QWidget *parent = nullptr);
	virtual ~NodeWindow() override;

private:
	bool shouldRedrawWatermark(const QSize& newSize);
	void drawWatermark();

public:
	void nodeWindowConfigure(QSharedPointer<Node> node);
	virtual void configure() = 0;

	QSharedPointer<Node> node() const;

	void focus();

	// Lifecycle stuff
protected:
	void nodeWindowRequestExit(const int returnValue = EXIT_SUCCESS);

	// Desktop stuff
protected:
	// Load position of window on screen
	void loadWindowGeometry();
	// Save position of window on screen
	void saveWindowGeometry();

	void updateWindowIcon();
	
	void setActivityStack(ActivityStack *activityStack);

	// QWidget interface
public:
	virtual void keyReleaseEvent(QKeyEvent *) override;
	virtual	void closeEvent(QCloseEvent *) override;
	virtual	void moveEvent(QMoveEvent *) override;
	virtual	void paintEvent(QPaintEvent *event) override;
	
	// Activity stuff
public slots:
	QString popPage();
	bool pushPage(const QString &title, const QStringList arguments = QStringList());
	QString swapPage(const QString &title);
	
	
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
