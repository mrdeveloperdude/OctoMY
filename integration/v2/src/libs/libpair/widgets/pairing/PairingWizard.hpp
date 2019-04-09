#ifndef PAIRINGWIZARD_HPP
#define PAIRINGWIZARD_HPP

#include "discovery/DiscoveryClient.hpp"
#include "uptime/ConfigureHelper.hpp"
#include <QWidget>


Q_DECLARE_METATYPE(QHostAddress)
Q_DECLARE_METATYPE(QHostAddress *)

namespace Ui
{
class PairingWizard;
}

class Node;
class PairingTable;
class PairingListModel;

class PairingEditButtonDelegate;
class QAbstractButton;

class AddressBook;
class LocalAddressList;



/**
 * @brief The PairingWizard class is a widget that is used by the nodes' UI when
 * they want input from user on what trust should be placed on each associate
 * that has been found through the discovery process.
 *
 * The widget has 3 main components:
 * + network settings - A widget to select address and port for communicating with associates
 * + associate list - A list of all associates found through discovery so far
 * + trust editor - A screen that allows user to tweak trust for one individual associate
 *
 * It also has a display of our current birth certificate and initiation of several 2-factor methods
 *
 */

class PairingWizard : public QWidget
{
	Q_OBJECT

private:
	Ui::PairingWizard *ui;
	QSharedPointer<Node> mNode;
	PairingListModel *mList;
	PairingEditButtonDelegate *mDelegate;
	QString mTemplate;
	QString mCurrentlyEditingID;
	int mTrustIndex;
	QTimer mPulsatingTrustTimer;
	ConfigureHelper mConfigureHelper;


public:
	explicit PairingWizard(QWidget *parent = nullptr);
	virtual ~PairingWizard() Q_DECL_OVERRIDE;

public:
	void configure(QSharedPointer<Node> node);

	// Convenience selectors
private:
	QSharedPointer<AddressBook> addressBook();
	QSharedPointer<LocalAddressList> localAddressList();

	// Helpers
private:
	void replaceText(NodeType type);

public:
	void setCurrentPage(QWidget *cur);
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

#endif
// PAIRINGWIZARD_HPP
