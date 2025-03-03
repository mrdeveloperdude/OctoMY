#ifndef LOGWIDGET_H
#define LOGWIDGET_H

#include <QWidget>

#include "uptime/ConfigureHelper.hpp"
#include "uptime/SharedPointerWrapper.hpp"

class LogModel;
class Node;

namespace Ui
{
class LogWidget;
}

class LogWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::LogWidget *ui;
	QSharedPointer<Node> mNode;
	ConfigureHelper mConfigureHelper;
	bool mScrollDirDown;

public:
	explicit LogWidget(QWidget *parent = nullptr);
	~LogWidget();

public:
	void configure(QSharedPointer<Node> node);
	void setLogDirection(const bool logUpwards);
};

#endif
// LOGWIDGET_HPP
