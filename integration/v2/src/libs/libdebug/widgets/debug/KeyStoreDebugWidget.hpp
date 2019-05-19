#ifndef KEYSTOREDEBUGWIDGET_HPP
#define KEYSTOREDEBUGWIDGET_HPP

#include "widgets/debug/SimpleTableDebugWidget.hpp"

#include "uptime/ConfigureHelper.hpp"

#include <QMap>
#include <QSharedPointer>

class Node;


class KeyStoreDebugWidget : public SimpleTableDebugWidget
{
	Q_OBJECT

private:
	QSharedPointer <Node> mNode;
	ConfigureHelper mConfigureHelper;

public:
	explicit KeyStoreDebugWidget(QWidget *parent = nullptr);
	virtual ~KeyStoreDebugWidget() Q_DECL_OVERRIDE;

public:
	void configure(QSharedPointer <Node> node);

public slots:
	void onRealtimeChanged(bool realtime) Q_DECL_OVERRIDE;
	void onTimer() Q_DECL_OVERRIDE;

};

#endif
// KEYSTOREDEBUGWIDGET_HPP
