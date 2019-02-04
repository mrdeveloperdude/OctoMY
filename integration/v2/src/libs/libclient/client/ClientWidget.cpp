#include "ClientWidget.hpp"

#include "uptime/MethodGate.hpp"


ClientWidget::ClientWidget(QWidget *parent)
	: QWidget(parent)
{
	OC_METHODGATE();
}


ClientWidget::~ClientWidget()
{
	OC_METHODGATE();
}
