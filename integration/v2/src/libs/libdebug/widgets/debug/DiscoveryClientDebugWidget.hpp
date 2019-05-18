#ifndef DISCOVERYCLIENTDEBUGWIDGET_HPP
#define DISCOVERYCLIENTDEBUGWIDGET_HPP

#include "uptime/ConfigureHelper.hpp"


#include <QWidget>
#include <QSharedPointer>
#include <QTimer>

class Node;

namespace Ui
{
class DiscoveryClientDebugWidget;
}

class DiscoveryClientDebugWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::DiscoveryClientDebugWidget *ui;
	ConfigureHelper mConfigureHelper;
	QSharedPointer <Node> mNode;
	QTimer mTimer;

public:
	explicit DiscoveryClientDebugWidget(QWidget *parent = nullptr);
	~DiscoveryClientDebugWidget();

public:
	void configure(QSharedPointer <Node> node);

public slots:
	void onUpdate();


	private slots:
	void on_pushButtonLogging_toggled(bool checked);
};

#endif
// DISCOVERYCLIENTDEBUGWIDGET_HPP
