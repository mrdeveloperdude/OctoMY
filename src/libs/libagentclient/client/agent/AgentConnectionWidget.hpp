#ifndef AGENTCONNECTIONWIDGET_HPP
#define AGENTCONNECTIONWIDGET_HPP

#include <QWidget>

namespace agent {
namespace client {


namespace Ui {
class AgentConnectionWidget;
}

class AgentConnectionWidget : public QWidget
{
	Q_OBJECT

public:
	explicit AgentConnectionWidget(QWidget *parent = nullptr);
	~AgentConnectionWidget();

private:
	Ui::AgentConnectionWidget *ui;
};

}
}

#endif // AGENTCONNECTIONWIDGET_HPP
