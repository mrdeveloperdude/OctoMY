#ifndef HUB_HPP
#define HUB_HPP

#include "basic/Node.hpp"

#include "basic/NodeLauncher.hpp"


/*

  A hub is categorized as a control together with remote, and use the same
  interface to control agents.

  But what separates a hub from a remote is that the hub represents a stationary
  automated program that may concert the operation of several agents. In
  contrast remote is used to control a single agent at the time.

*/


class CommsSession;
class HubWindow;

class Hub: public Node{
		Q_OBJECT
	public:
		HubWindow *window;
	public:

		explicit Hub(NodeLauncher<Hub> &launcher, QObject *parent = nullptr);
		virtual ~Hub();

		virtual QWidget *showWindow();


};

#endif // HUB_HPP
