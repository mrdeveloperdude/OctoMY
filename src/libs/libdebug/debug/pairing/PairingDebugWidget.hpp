#ifndef PAIRINGDEBUGWIDGET_H
#define PAIRINGDEBUGWIDGET_H

#include "uptime/ConfigureHelper.hpp"
#include "uptime/SharedPointerWrapper.hpp"

#include <QTimer>
#include <QWidget>

class Node;
class PairingTrustActivity;

namespace Ui
{
class PairingDebugWidget;
}

class PairingDebugWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::PairingDebugWidget *ui;
	QSharedPointer <Node> mNode;
	QSharedPointer <PairingTrustActivity> mTrustWidget;
	ConfigureHelper mConfigureHelper;

public:
	explicit PairingDebugWidget(QWidget *parent = nullptr);
	virtual ~PairingDebugWidget() override;

public:
	void configure(QSharedPointer <Node> node);

public slots:
	void onUpdate();

};

#endif
// PAIRINGDEBUGWIDGET_HPP


