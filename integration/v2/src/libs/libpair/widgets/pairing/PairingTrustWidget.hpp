#ifndef PAIRINGTRUSTWIDGET_HPP
#define PAIRINGTRUSTWIDGET_HPP

#include "uptime/ConfigureHelper.hpp"
#include "TrustLevel.hpp"

#include <QWidget>
#include <QTimer>

class Node;
class QAbstractButton;
class Associate;

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
	QSharedPointer<Node> mNode;
	ConfigureHelper mConfigureHelper;
	QString mCurrentID;

public:
	explicit PairingTrustWidget(QWidget *parent = nullptr);
	virtual ~PairingTrustWidget() Q_DECL_OVERRIDE;


public:
	void configure(QSharedPointer<Node> node);

public:
	void updatePulsating();

	void startEdit(QSharedPointer<Associate> peer);
	TrustLevel selectedTrustLevel() const;
	void selectTrustLevel(TrustLevel level);

protected:
	void showEvent(QShowEvent *) Q_DECL_OVERRIDE;
	void hideEvent(QHideEvent *) Q_DECL_OVERRIDE;


private slots:
	void onPulsatingTrustTimer();
	void onTrustButtonClicked(QAbstractButton *);

private slots:
	void on_pushButtonSaveEdits_clicked();
	void on_pushButtonRemove_clicked();

signals:
	void editComplete(QString id, TrustLevel level, bool save);
	void remove(QString id);

};

#endif
// PAIRINGTRUSTWIDGET_HPP
