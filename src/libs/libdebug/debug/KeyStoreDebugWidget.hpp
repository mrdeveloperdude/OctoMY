#ifndef KEYSTOREDEBUGWIDGET_HPP
#define KEYSTOREDEBUGWIDGET_HPP

#include "debug/SimpleTableDebugWidget.hpp"

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
	virtual ~KeyStoreDebugWidget() override;

public:
	void configure(QSharedPointer <Node> node);

public slots:
	void onRealtimeChanged(bool realtime) override;
	void onTimer() override;

};

#endif
// KEYSTOREDEBUGWIDGET_HPP
