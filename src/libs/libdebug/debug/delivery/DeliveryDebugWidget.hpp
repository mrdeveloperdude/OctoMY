#ifndef DELIVERYDEBUGWIDGET_H
#define DELIVERYDEBUGWIDGET_H

#include "uptime/SharedPointerWrapper.hpp"
#include "uptime/ConfigureHelper.hpp"
#include "delivery/BirthControl.hpp"

#include <QWidget>

class Node;

namespace Ui {
class DeliveryDebugWidget;
}

class DeliveryDebugWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::DeliveryDebugWidget *ui;
	QSharedPointer<Node> mNode;
	ConfigureHelper mConfigureHelper;
	BirthControl mBirthControl;

public:
	explicit DeliveryDebugWidget(QWidget *parent = nullptr);
	~DeliveryDebugWidget();

public:
	void configure(QSharedPointer <Node> node);
	
	void updateUI();
	
private slots:
	void scrapeWoom();
	void reload();

};

#endif // DELIVERYDEBUGWIDGET_H
