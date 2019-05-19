#ifndef PAIRINGTRUSTWIDGET_HPP
#define PAIRINGTRUSTWIDGET_HPP

#include "uptime/ConfigureHelper.hpp"

#include <QWidget>

class QAbstractButton;

namespace Ui
{
class PairingTrustWidget;
}

class PairingTrustWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::PairingTrustWidget *ui;
	QTimer mPulsatingTrustTimer;
	ConfigureHelper mConfigureHelper;

public:
	explicit PairingTrustWidget(QWidget *parent = nullptr);
	virtual ~PairingTrustWidget() Q_DECL_OVERRIDE;

public:
	void configure(QSharedPointer<Node> node);

public:
	void updatePulsating();
	void startEdit();

	protected:
		void showEvent(QShowEvent *) Q_DECL_OVERRIDE;
		void hideEvent(QHideEvent *) Q_DECL_OVERRIDE;


private slots:
	void onPulsatingTrustTimer();
	void onTrustButtonClicked(QAbstractButton *);

private slots:
	void on_pushButtonSaveEdits_clicked();
	void on_pushButtonRemove_clicked();

};

#endif
// PAIRINGTRUSTWIDGET_HPP
