#ifndef COMMSDEBUGWIDGET_H
#define COMMSDEBUGWIDGET_H

#include "uptime/ConfigureHelper.hpp"


#include <QWidget>
#include "uptime/SharedPointerWrapper.hpp"

class Node;
namespace Ui
{
class CommsDebugWidget;
}

class CommsDebugWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::CommsDebugWidget *ui;
	ConfigureHelper mConfigureHelper;
	QSharedPointer <Node> mNode;

public:
	explicit CommsDebugWidget(QWidget *parent = nullptr);
	~CommsDebugWidget();

public:
	void configure(QSharedPointer <Node> node);

};

#endif
// COMMSDEBUGWIDGET_HPP
