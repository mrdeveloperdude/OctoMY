#ifndef LOCALADDRESSESDEBUGWIDGET_H
#define LOCALADDRESSESDEBUGWIDGET_H

#include "debug/table/SimpleTableDebugWidget.hpp"

#include "uptime/ConfigureHelper.hpp"

#include <QMap>
#include "uptime/SharedPointerWrapper.hpp"

class Node;


class LocalAddressesDebugWidget : public SimpleTableDebugWidget
{
	Q_OBJECT

private:
	QSharedPointer <Node> mNode;
	ConfigureHelper mConfigureHelper;

public:
	explicit LocalAddressesDebugWidget(QWidget *parent = nullptr);
	virtual ~LocalAddressesDebugWidget() override;

public:
	void configure(QSharedPointer <Node> node);

public slots:
	void onRealtimeChanged(bool realtime) override;
	void onTimer() override;

};

#endif
// LOCALADDRESSESDEBUGWIDGET_HPP
