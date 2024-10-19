#ifndef PAIRINGACTIVITY_H
#define PAIRINGACTIVITY_H

#include "uptime/ConfigureHelper.hpp"
#include "node/NodeType.hpp"
#include "components/navigation/Activity.hpp"

#include <QWidget>
#include <QHostAddress>

Q_DECLARE_METATYPE(QHostAddress)
Q_DECLARE_METATYPE(QHostAddress *)

namespace Ui
{
class PairingActivity;
}

class Node;
class PairingTable;
class PairingListModel;

class PairingEditButtonDelegate;
class QAbstractButton;

class AddressBook;
class LocalAddressList;



/**
 * @brief The PairingActivity class is a widget that is used by the nodes' UI when
 * they want input from user on what trust should be placed on each associate
 * that has been found through the discovery process.
 *
 * The widget has 3 main components:
 * + network settings - A widget to select address and port for communicating with associates
 * + associate list - A list of all associates found through discovery so far
 * + trust editor - A screen that allows user to edit trust for one individual associate
 *
 * It also has a display of our current birth certificate and initiation of several 2-factor methods
 */

class PairingActivity : public Activity
{
	Q_OBJECT

private:
	Ui::PairingActivity *ui;
	QSharedPointer<Node> mNode;
	PairingListModel *mList;
	PairingEditButtonDelegate *mDelegate;
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
// PairingActivity_HPP
