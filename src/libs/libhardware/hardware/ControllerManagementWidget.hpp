#ifndef CONTROLLERMANAGEMENTWIDGET_HPP
#define CONTROLLERMANAGEMENTWIDGET_HPP

#include <QWidget>

#include "uptime/SharedPointerWrapper.hpp"

class ControllerHandler;

namespace Ui {
class ControllerManagementWidget;
}

class ControllerManagementWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::ControllerManagementWidget *ui;
	QSharedPointer<ControllerHandler> mHandler;
	bool mDebug{true};

public:
	explicit ControllerManagementWidget(QWidget *parent = nullptr);
	~ControllerManagementWidget();

public:
	void configure(QSharedPointer<ControllerHandler>);

private slots:
	void updateController();

};

#endif // CONTROLLERMANAGEMENTWIDGET_HPP
