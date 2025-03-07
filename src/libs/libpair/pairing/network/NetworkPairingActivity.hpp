#ifndef NETWORKPAIRINGACTIVITY_H
#define NETWORKPAIRINGACTIVITY_H

#include "uptime/ConfigureHelper.hpp"
#include "node/NodeType.hpp"
#include "components/navigation/Activity.hpp"

#include <QWidget>
#include <QHostAddress>


namespace Ui
{
class NetworkPairingActivity;
}

class Node;
class PairingTable;
class PairingListModel;

class PairingEditButtonDelegate;
class QAbstractButton;

class AddressBook;
class LocalAddressList;



/**
 * @brief The NetworkPairingActivity class is a scree where the user can assign 
 * trust to each associate that has been found through the discovery process
 * with explicit focus on nodes discovered via IP networks (wifi/lan/zoo).
 * 
 * NOTE: This will be superceeded by PairingActivity which aims to incorporate
 *       all nodes discovered via any carrier.
 *
 * The widget has 3 main components:
 * + network settings - A widget to select address and port for communicating with associates
 * + associate list - A list of all associates found through discovery so far
 * + trust editor - A screen that allows user to edit trust for one individual associate
 *
 * It also has a display of our current birth certificate and initiation of several 2-factor methods
 */

class NetworkPairingActivity : public Activity
{
	Q_OBJECT

private:
	Ui::NetworkPairingActivity *ui;
	QSharedPointer<Node> mNode;
	PairingListModel *mList{nullptr};
	PairingEditButtonDelegate *mDelegate{nullptr};
	QString mTemplate;
	QString mCurrentlyEditingID;
	static const QString mEncouragmentTextControl;
	static const QString mEncouragmentTextAgent;
	bool mDebug{false};

	ConfigureHelper mConfigureHelper;


public:
	explicit NetworkPairingActivity(QWidget *parent = nullptr);
	virtual ~NetworkPairingActivity() override;

public:
	void configure(QSharedPointer<Node> node);
	
	// Virtual activity mechanism
protected:
	void popImpl(const QString &returnActivity, const QStringList returnArguments = QStringList()) override;
	void pushImpl(const QStringList arguments = QStringList()) override;
	
	// Convenience selectors
private:
	QSharedPointer<AddressBook> addressBook();
	QSharedPointer<LocalAddressList> localAddressList();
	bool isPaierd() const;

	// Helpers
private:
	void replaceText(NodeType type);
	void hookStartEdit();

public:
	void startEdit(const QString &id);
	QSharedPointer<Node> node();
	void updateNetworkSettings();

protected:
	void showEvent(QShowEvent *) override;
	void hideEvent(QHideEvent *) override;

private slots:
	void onNetworkSettingsChange(QHostAddress address, quint16 port, bool valid);

private slots:
	void complete();
	void cameraPair();
	void refresh();

};

#endif
// NetworkPairingActivity_HPP
