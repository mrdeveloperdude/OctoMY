#ifndef CARRIERDEBUGWIDGET_H
#define CARRIERDEBUGWIDGET_H

#include "uptime/ConfigureHelper.hpp"


#include <QWidget>
#include <QSharedPointer>

class Node;

namespace Ui
{
class CarrierDebugWidget;
}

class CarrierDebugWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::CarrierDebugWidget *ui;
	ConfigureHelper mConfigureHelper;
	QSharedPointer <Node> mNode;

public:
	explicit CarrierDebugWidget(QWidget *parent = nullptr);
	~CarrierDebugWidget();

public:
	void configure(QSharedPointer <Node> node);

public slots:
	void onUpdate();

};

#endif
// CARRIERDEBUGWIDGET_HPP
