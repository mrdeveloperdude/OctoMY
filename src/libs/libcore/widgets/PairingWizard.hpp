#ifndef PAIRINGWIZARD_HPP
#define PAIRINGWIZARD_HPP

#include "discovery/DiscoveryClient.hpp"

#include <QWidget>



namespace Ui
{
class PairingWizard;
}

class Node;
class PairingTable;
class PairingListModel;

class PairingEditButtonDelegate;
class QAbstractButton;

class PairingWizard : public QWidget
{
	Q_OBJECT

private:
	Ui::PairingWizard *ui;
	QSharedPointer<Node> mNode;
	PairingListModel *mList;
	PairingEditButtonDelegate *mDelegate;
	QString mTemplate;
	QString mCurrentlyEditing;
	int mTrustIndex;

	QTimer mPulsatingTrustTimer;
public:
	explicit PairingWizard(QWidget *parent = 0);
	virtual ~PairingWizard();
public:

	void configure(QSharedPointer<Node> node);

public:
	void reset();

	void startEdit(int row);

	QSharedPointer<Node> node();

	void updateNetworkSettings();

	void updatePulsating();

protected:

	void showEvent(QShowEvent *) Q_DECL_OVERRIDE;
	void hideEvent(QHideEvent *) Q_DECL_OVERRIDE;

private slots:

	void onNetworkSettingsChange(QHostAddress address, quint16 port, bool valid);
	void onTrustButtonClicked(QAbstractButton *);
	void onPulsatingTrustTimer();


private slots:
	void on_pushButtonMaybeOnward_clicked();
	void on_pushButtonTryAgain_clicked();
	void on_pushButtonDone_clicked();
	void on_pushButtonCameraPair_clicked();
	void on_pushButtonSaveEdits_clicked();
	void on_pushButtonRemove_clicked();
	void on_pushButtonRefresh_clicked();

signals:
	void done();
};

#endif // PAIRINGWIZARD_HPP
