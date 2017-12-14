#ifndef HUB_HPP
#define HUB_HPP

#include "node/Node.hpp"

#include "node/NodeLauncher.hpp"


/*

  A hub is categorized as a control together with remote, and use the same
  interface to control agents.

  But what separates a hub from a remote is that the hub represents a stationary
  automated program that may concert the operation of several agents. In
  contrast remote is used to control a single agent at the time.

*/


class CommsSession;
class HubWindow;

class Hub: public Node, public QEnableSharedFromThis<Hub>
{
	Q_OBJECT
public:
	QSharedPointer<HubWindow> mWindow;

public:

	explicit Hub(NodeLauncher<Hub> &launcher, QObject *parent = nullptr);
	virtual ~Hub();

public:
	virtual QSharedPointer<QWidget> showWindow();


	QSharedPointer<Node> sharedThis() Q_DECL_OVERRIDE;

};

#endif // HUB_HPP
