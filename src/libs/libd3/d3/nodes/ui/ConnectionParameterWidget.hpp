#ifndef CONNECTIONPARAMETERWIDGET_HPP
#define CONNECTIONPARAMETERWIDGET_HPP

#include <QWidget>
#include "uptime/SharedPointerWrapper.hpp"

class Connection;

namespace Ui {
class ConnectionParameterWidget;
}

class ConnectionParameterWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::ConnectionParameterWidget *ui;
	QSharedPointer<Connection> mConnection;

public:
	explicit ConnectionParameterWidget(QWidget *parent = nullptr);
	~ConnectionParameterWidget();

public:
	void setConnection(QSharedPointer<Connection> connection);

private slots:
	void expressionChanged(const QString &expression);
	
};

#endif // CONNECTIONPARAMETERWIDGET_HPP
