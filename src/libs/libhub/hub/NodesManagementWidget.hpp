#ifndef NODESMANAGEMENTWIDGET_HPP
#define NODESMANAGEMENTWIDGET_HPP

#include <QSharedPointer>
#include <QThread>
#include <QTimer>
#include <QWidget>


#include "node/NodeFactory.hpp"

class Settings;


namespace Ui {
class NodesManagementWidget;
}

class NodesManagementWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::NodesManagementWidget *ui;
	QTimer mSummaryTimer;
	QMenu *mAddLocalMenu{nullptr};
	NodeFactory mNodeFactory;

public:
	explicit NodesManagementWidget(QWidget *parent = nullptr);
	~NodesManagementWidget();

public:
	void configure(QSharedPointer<Settings> settings);
	void setListening(bool on);
	void update();

private slots:
	void showAddLocal();
	void onSummaryTimer();
	void addLocalAgent();
	void addLocalRemote();

};

#endif // NODESMANAGEMENTWIDGET_HPP
