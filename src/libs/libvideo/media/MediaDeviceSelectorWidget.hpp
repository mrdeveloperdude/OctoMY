#ifndef MEDIADEVICESELECTORWIDGET_H
#define MEDIADEVICESELECTORWIDGET_H

#include <QWidget>

#include "uptime/SharedPointerWrapper.hpp"

class MediaDeviceManager;
class MediaDeviceModel;
class Node;

namespace Ui {
class MediaDeviceSelectorWidget;
}

class MediaDeviceSelectorWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::MediaDeviceSelectorWidget *ui;
	QSharedPointer <MediaDeviceManager> mManager;
	QSharedPointer <MediaDeviceModel> mModel;
	QSharedPointer<Node> mNode;

public:
	explicit MediaDeviceSelectorWidget(QWidget *parent = nullptr);
	~MediaDeviceSelectorWidget();
	
public:
	void configure(QSharedPointer<Node> node);

public slots:
	void togglePreview(bool preview);

};

#endif // MEDIADEVICESELECTORWIDGET_H
