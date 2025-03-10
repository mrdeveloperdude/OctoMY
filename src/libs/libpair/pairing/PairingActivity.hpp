#ifndef PAIRINGACTIVITY_HPP
#define PAIRINGACTIVITY_HPP


#include "components/navigation/Activity.hpp"
#include "log/LogType.hpp"
#include "node/NodeType.hpp"
#include "uptime/ConfigureHelper.hpp"

#include <QWidget>
#include <QHostAddress>

Q_DECLARE_METATYPE(QHostAddress)
Q_DECLARE_METATYPE(QHostAddress *)


namespace Ui {
class PairingActivity;
}


class AddressBook;
class DiscoveryClient;
class LocalAddressList;
class Node;
class PairingEditButtonDelegate;
class PairingListModel;
class PairingTable;
class QAbstractButton;


/**
 * @brief The PairingActivity class is a scree where the user can assign 
 * trust to each associate that has been found through the discovery process.
 * 
 * This builds on the NetworkPairingActivity which did the same but only for 
 * nodes discovered over IP networks.
  *
 * The widget has 3 main components:
 * + network settings - A widget to select address and port for communicating with associates
 * + associate list - A list of all associates found through discovery so far
 * + trust editor - A screen that allows user to edit trust for one individual associate
 *
 * It also has a display of our current birth certificate and initiation of several 2-factor methods
 */


class PairingActivity: public Activity
{
	Q_OBJECT
	
private:
	Ui::PairingActivity *ui;
	QSharedPointer<Node> mNode;
	QSharedPointer<DiscoveryClient> mDiscoveryClient;
	PairingListModel *mList{nullptr};
	PairingEditButtonDelegate *mDelegate{nullptr};
	QString mTemplate;
	QString mCurrentlyEditingID;
	static const QString mEncouragmentTextControl;
	static const QString mEncouragmentTextAgent;
	bool mDebug{false};
	
	ConfigureHelper mConfigureHelper;
	
	
public:
	explicit PairingActivity(QWidget *parent = nullptr);
	virtual ~PairingActivity() override;
	
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
	void updateDiscoveryMandate();
	void log(const QString &text, LogType type=LOG_TYPE_INFO) const;
	
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
	void refresh();

};

#endif // PAIRINGACTIVITY_HPP
