#ifndef COMMSDEBUGWIDGET_HPP
#define COMMSDEBUGWIDGET_HPP

#include "uptime/ConfigureHelper.hpp"


#include <QWidget>
#include <QSharedPointer>

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
