#ifndef PAIRINGDEBUGWIDGET_HPP
#define PAIRINGDEBUGWIDGET_HPP

#include "uptime/ConfigureHelper.hpp"


#include <QWidget>
#include <QSharedPointer>
#include <QTimer>

class Node;
class PairingTrustWidget;

namespace Ui
{
class PairingDebugWidget;
}

class PairingDebugWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::PairingDebugWidget *ui;
	ConfigureHelper mConfigureHelper;
	QSharedPointer <Node> mNode;
	QSharedPointer <PairingTrustWidget> mTrustWidget;

public:
	explicit PairingDebugWidget(QWidget *parent = nullptr);
	virtual ~PairingDebugWidget() Q_DECL_OVERRIDE;

public:
	void configure(QSharedPointer <Node> node);

public slots:
	void onUpdate();

};

#endif
// PAIRINGDEBUGWIDGET_HPP


