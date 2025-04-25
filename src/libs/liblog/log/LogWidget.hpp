#ifndef LOGWIDGET_H
#define LOGWIDGET_H

#include <QWidget>

#include "uptime/ConfigureHelper.hpp"
#include "uptime/SharedPointerWrapper.hpp"

class LogModel;
class LogStorage;
class Node;
class QSortFilterProxyModel;

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
	QSharedPointer<LogModel> mModel{nullptr};
	QSortFilterProxyModel *mProxy{nullptr};
	bool mScrollDirection{false};
	ConfigureHelper mConfigureHelper;

public:
	explicit LogWidget(QWidget *parent = nullptr);
	~LogWidget();
	
private:
	void setStorage(QSharedPointer<LogStorage> storage);
	void scrollToEnd();

public:
	void configure(QSharedPointer<LogStorage> storage);
	void configure(QSharedPointer<Node> node);
	QSharedPointer<LogModel> model();
	void setLogDirection(const bool logUpwards);
	void setEnableFilter(const bool showFilter);
};

#endif
// LOGWIDGET_HPP
