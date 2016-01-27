#include "ClientModel.hpp"

#include "hub/Client.hpp"

#include <QIcon>
#include <QPixmap>

/*


QIcon ic(QPixmap::fromImage(tint(QImage(":/icons/octomy_logo_bare.svg"),nl->connected?QColor(0,192,0):QColor(192,0,0),1.0)));

Client *c=(Client *)item->data().value<void *>();
if(0!=c){
	appendLog("OPENING CLINET "+c->getListText());
	ClientWindow *cw=new ClientWindow(c,0);
	cw->show();
}
else{
	appendLog("could not find client for item");
}
	*/

/*
	ui->plainTextEditSummary->setPlainText(comms->getSummary());
	const ReliabilitySystem rs;//=cs->getReliabilitySystem();
	const int sentPackets=rs.sentPackets()-lastSentPackets;
	const int receivedPackets=rs.receivedPackets()-lastReceivedPackets;
	const int lostPackets=rs.lostPackets()-lastLostPackets;
	const int ackedPackets=rs.ackedPackets()-lastAckedPackets;
	lastSentPackets+=sentPackets;
	lastReceivedPackets+=receivedPackets;
	lastLostPackets+=lostPackets;
	lastAckedPackets+=ackedPackets;
	appendGraphData(rs.roundTripTime(), sentPackets, receivedPackets, lostPackets, ackedPackets, rs.sentBandwidth(), rs.ackedBandwidth());
	*/




ClientModel::ClientModel(QMap<quint64, Client *> &clients, QObject *parent)
	: QAbstractItemModel(parent)
	, clients(clients)
	, services(QPixmap(":/images/services.png"))
	, tree(new QVector<Node>(clients.size(), Node(0)))
{

}

ClientModel::~ClientModel(){
	delete tree;
}

QModelIndex ClientModel::index(int row, int column, const QModelIndex &parent) const{
	if (row < rc() && row >= 0 && column < cc() && column >= 0) {
		Node *parentNode = static_cast<Node*>(parent.internalPointer());
		Node *childNode = node(row, parentNode);
		if (childNode){
			return createIndex(row, column, childNode);
		}
	}
	return QModelIndex();
}

QModelIndex ClientModel::parent(const QModelIndex &child) const{
	if (child.isValid()) {
		Node *childNode = static_cast<Node*>(child.internalPointer());
		Node *parentNode = parent(childNode);
		if (parentNode){
			return createIndex(row(parentNode), 0, parentNode);
		}
	}
	return QModelIndex();
}

int ClientModel::rowCount(const QModelIndex &parent) const{
	return (parent.isValid() && parent.column() != 0) ? 0 : rc();
}

int ClientModel::columnCount(const QModelIndex &parent) const{
	Q_UNUSED(parent);
	return cc();
}

QVariant ClientModel::data(const QModelIndex &index, int role) const{
	if (!index.isValid()){
		return QVariant();
	}
	if (role == Qt::DisplayRole){
		return QVariant("Item " + QString::number(index.row()) + ":" + QString::number(index.column()));
	}
	if (role == Qt::DecorationRole) {
		if (index.column() == 0){
			return iconProvider.icon(QFileIconProvider::Folder);
		}
		return iconProvider.icon(QFileIconProvider::File);
	}
	return QVariant();
}

QVariant ClientModel::headerData(int section, Qt::Orientation orientation, int role) const{
	if (role == Qt::DisplayRole){
		return QString::number(section);
	}
	if (role == Qt::DecorationRole){
		return QVariant::fromValue(services);
	}
	return QAbstractItemModel::headerData(section, orientation, role);
}

bool ClientModel::hasChildren(const QModelIndex &parent) const{
	if (parent.isValid() && parent.column() != 0){
		return false;
	}
	return rc() > 0 && cc() > 0;
}

Qt::ItemFlags ClientModel::flags(const QModelIndex &index) const{
	if (!index.isValid()){
		return 0;
	}
	return Qt::ItemIsDragEnabled|QAbstractItemModel::flags(index);
}

ClientModel::Node *ClientModel::node(int row, Node *parent) const{
	if (parent && !parent->children){
		parent->children = new QVector<Node>(rc(), Node(parent));
	}
	QVector<Node> *v = parent ? parent->children : tree;
	return const_cast<Node*>(&(v->at(row)));
}

ClientModel::Node *ClientModel::parent(Node *child) const{
	return child ? child->parent : 0;
}

int ClientModel::row(Node *node) const{
	const Node *first = node->parent ? &(node->parent->children->at(0)) : &(tree->at(0));
	return node - first;
}
