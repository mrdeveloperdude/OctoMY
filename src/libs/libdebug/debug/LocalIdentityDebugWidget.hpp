#ifndef LOCALIDENTITYDEBUGWIDGET_HPP
#define LOCALIDENTITYDEBUGWIDGET_HPP

#include "debug/SimpleTableDebugWidget.hpp"

#include "uptime/ConfigureHelper.hpp"

#include <QMap>
#include <QSharedPointer>

class Node;


class LocalIdentityDebugWidget : public SimpleTableDebugWidget
{
	Q_OBJECT
private:
	QSharedPointer <Node> mNode;
	ConfigureHelper mConfigureHelper;

public:
	explicit LocalIdentityDebugWidget(QWidget *parent = nullptr);
	virtual ~LocalIdentityDebugWidget() Q_DECL_OVERRIDE;

public:
	void configure(QSharedPointer <Node> node);

public slots:
	void onRealtimeChanged(bool realtime) Q_DECL_OVERRIDE;
	void onTimer() Q_DECL_OVERRIDE;

};

#endif
// LOCALIDENTITYDEBUGWIDGET_HPP
