#include "AgentConnectionWidget.hpp"
#include "ui_AgentConnectionWidget.h"

namespace agent {
namespace client {
AgentConnectionWidget::AgentConnectionWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::AgentConnectionWidget)
{
	ui->setupUi(this);
}

AgentConnectionWidget::~AgentConnectionWidget()
{
	delete ui;
}



}
}
