#ifndef HUB_HPP
#define HUB_HPP

#include "basic/Node.hpp"

#include "basic/NodeLauncher.hpp"

class Client;
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
