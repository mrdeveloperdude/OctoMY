#ifndef CONTROLLERSTATSWIDGET_HPP
#define CONTROLLERSTATSWIDGET_HPP

#include <QWidget>

#include "uptime/SharedPointerWrapper.hpp"

class ControllerHandler;

namespace Ui {
class ControllerStatsWidget;
}

class ControllerStatsWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::ControllerStatsWidget *ui;
	QSharedPointer<ControllerHandler> mHandler;
	bool mDebug{true};

public:
	explicit ControllerStatsWidget(QWidget *parent = nullptr);
	~ControllerStatsWidget();

public:
	void configure(QSharedPointer<ControllerHandler>);

private slots:
	void updateController();
};

#endif // CONTROLLERSTATSWIDGET_HPP
