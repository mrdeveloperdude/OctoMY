#ifndef DISCOVERYCLIENTDEBUGWIDGET_H
#define DISCOVERYCLIENTDEBUGWIDGET_H

#include "uptime/ConfigureHelper.hpp"


#include <QWidget>
#include "uptime/SharedPointerWrapper.hpp"
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
	virtual ~DiscoveryClientDebugWidget() override;

public:
	void configure(QSharedPointer <Node> node);

public slots:
	void onUpdate();

private slots:
	void on_pushButtonLogging_toggled(bool checked);
	void on_pushButtonDiscover_clicked();
};

#endif
// DISCOVERYCLIENTDEBUGWIDGET_HPP
