#ifndef LOCALADDRESSESDEBUGWIDGET_HPP
#define LOCALADDRESSESDEBUGWIDGET_HPP

#include "widgets/debug/SimpleTableDebugWidget.hpp"

#include "uptime/ConfigureHelper.hpp"

#include <QMap>
#include <QSharedPointer>

class Node;


class LocalAddressesDebugWidget : public SimpleTableDebugWidget
{
	Q_OBJECT

private:
	QSharedPointer <Node> mNode;
	ConfigureHelper mConfigureHelper;

public:
	explicit LocalAddressesDebugWidget(QWidget *parent = nullptr);
	virtual ~LocalAddressesDebugWidget() Q_DECL_OVERRIDE;

public:
	void configure(QSharedPointer <Node> node);

public slots:
	void onRealtimeChanged(bool realtime) Q_DECL_OVERRIDE;
	void onTimer() Q_DECL_OVERRIDE;

};

#endif
// LOCALADDRESSESDEBUGWIDGET_HPP
