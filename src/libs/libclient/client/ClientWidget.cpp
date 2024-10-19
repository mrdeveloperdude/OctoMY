#include "ClientWidget.hpp"

#include "uptime/MethodGate.hpp"


ClientWidget::ClientWidget(QWidget *parent)
	: Activity(parent)
{
	OC_METHODGATE();
}


ClientWidget::~ClientWidget()
{
	OC_METHODGATE();
}
