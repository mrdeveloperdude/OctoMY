#include "Graph.hpp"

#include "Node.hpp"
#include "Connection.hpp"
#include "Tri.hpp"

#include <QDebug>
#include <QFile>
#include <QIODevice>
#include <QRandomGenerator>
#include <QSharedPointer>
#include <QVector2D>

static const QString MAGIC{"GRAPH"};
static const quint32 VERSION{1};

Graph::Graph()
: mStyle(new Style())
{
}


bool Graph::hasCycleWorker(QSharedPointer<Node> node, QSet<QString> &visited, QSet<QString> &recStack, const QSharedPointer<Node> &nodeA, const QSharedPointer<Node> &nodeB) const {
	QString nodeId = node->id();
	if (!visited.contains(nodeId)) {
		visited.insert(nodeId);
		recStack.insert(nodeId);
		for (const auto &conn : mConnections) {
			if (conn->from()->id() == nodeId) {
				if (!visited.contains(conn->to()->id()) && hasCycleWorker(conn->to(), visited, recStack, nodeA, nodeB)) {
					return true;
				} else if (recStack.contains(conn->to()->id())) {
					return true;
				}
			}
		}
		if (nodeId == nodeA->id()) {
			if (!visited.contains(nodeB->id()) && hasCycleWorker(nodeB, visited, recStack, nodeA, nodeB)) {
				return true;
			} else if (recStack.contains(nodeB->id())) {
				return true;
			}
		}
	}
	recStack.remove(nodeId);
	return false;
}


void Graph::taint(const QString &reason){
	if(mLastTaintReason != reason){
		//qDebug() << "Graph tainted by" << reason;
		mLastTaintReason = reason;
	}
	mTainted = true;
}


void Graph::unTaint(const QString &reason){
	qDebug()<<"Graph un-tainted by"<<reason;
	mTainted = false;
}


bool Graph::isTainted() const{
	return mTainted;
}


bool Graph::wouldCreateCycle(const QSharedPointer<Node> &nodeA, const QSharedPointer<Node> &nodeB) const {
	QSet<QString> visited;
	QSet<QString> recStack;
	for (const auto &node : mNodes) {
		if (hasCycleWorker(node, visited, recStack, nodeA, nodeB)) {
			return true;
		}
	}
	return false;
}


void Graph::addNode(QSharedPointer<Node> node){
	if(mNodes.contains(node)){
		return;
	}
	mNodes.push_back(node);
	mTopologyChanged = true;
	taint("addNode");
}


ConnectionStatus Graph::addConnection(QSharedPointer<Node> fromNode,QSharedPointer<Node> toNode){
	if((fromNode == toNode) || fromNode.isNull() || toNode.isNull() ){
		return ConnectionStatus(false, "Impossible connection could not be made");
	}
	if(connectionExists(fromNode, toNode, false)){
		return ConnectionStatus(false, "Nodes are already connected");
	}
	if(!mAllowCycles && wouldCreateCycle(fromNode, toNode)){
		//, bool directionMatters
		return ConnectionStatus(false, "Connecting these nodes would form an invalid loop");
	}
	mConnections.push_back(QSharedPointer<Connection>::create(sharedFromThis(), fromNode, toNode));
	mTopologyChanged = true;
	taint("addConnection");
	return ConnectionStatus(true);
}


ConnectionStatus Graph::addTri(QSharedPointer<Node> n1, QSharedPointer<Node> n2, QSharedPointer<Node> n3){
	if((n1 == n2) || (n2 == n3) || (n3 == n1) || n1.isNull() || n2.isNull() || n2.isNull()){
		return ConnectionStatus(false, "Impossible tri could not be made");
	}
	if(triExists(n1, n2, n3, false)){
		return ConnectionStatus(false, "Tri already exists");
	}
	mTris.push_back(QSharedPointer<Tri>::create(sharedFromThis(), n1, n2, n3));
	mTopologyChanged = true;
	taint("addTri");
	return ConnectionStatus(true);
}


void Graph::disconnectNode(QSharedPointer<Node> node){
	QVector<QSharedPointer<Connection> >removeUs;
	for(auto &connection:mConnections){
		if(connection->from() == node){
			removeUs<<connection;
		}
		else if(connection->to() == node){
			removeUs<<connection;
		}
	}
	bool anyRemoved=false;
	for(auto &connection:removeUs){
		removeConnection(connection);
		anyRemoved=true;
	}
	if(anyRemoved){
		mTopologyChanged = true;
		taint("disconnectNode");
	}
	removeUs.clear();
}


void Graph::disTriNode(QSharedPointer<Node> node){
	QVector<QSharedPointer<Tri> >removeUs;
	for(auto &tri:mTris){
		if(tri->node1() == node){
			removeUs<<tri;
		}
		else if(tri->node2() == node){
			removeUs<<tri;
		}
		else if(tri->node3() == node){
			removeUs<<tri;
		}
	}
	bool anyRemoved=false;
	for(auto &tri:removeUs){
		removeTri(tri);
		anyRemoved=true;
	}
	if(anyRemoved){
		mTopologyChanged = true;
		taint("disTriNode");
	}
	removeUs.clear();
}



void Graph::removeNode(QSharedPointer<Node> node){
	disconnectNode(node);
	disTriNode(node);
	if(mNodes.contains(node)){
		mNodes.removeOne(node);
		mTopologyChanged = true;
		taint("removeNode");
	}
}


void Graph::removeConnection(QSharedPointer<Connection> connection){
	if(mConnections.contains(connection)){
		mConnections.removeOne(connection);
		mTopologyChanged = true;
		taint("removeConnection");
	}
}

void Graph::removeTri(QSharedPointer<Tri> tri){
	if(mTris.contains(tri)){
		mTris.removeOne(tri);
		mTopologyChanged = true;
		taint("removeTri");
	}
}

QSharedPointer<Connection> Graph::connectionFor(QSharedPointer<Node> fromNode,QSharedPointer<Node> toNode, bool directionMatters) const{
	for(auto const &connection:mConnections){
		if((connection->from() == fromNode) && (connection->to() == toNode)){
			return connection;
		}
		if(!directionMatters && (connection->from() == toNode) && (connection->to() == fromNode)){
			return connection;
		}
	}
	return nullptr;
}


QSharedPointer<Tri> Graph::triFor(QSharedPointer<Node> n1, QSharedPointer<Node> n2, QSharedPointer<Node> n3, bool directionMatters) const{
	if(directionMatters){
		for(auto const &tri:mTris){
			if((tri->node1() == n1) && (tri->node2() == n2) && (tri->node3() == n3)){
				return tri;
			}
		}
	}
	else{
		for(auto const &tri:mTris){
			if((tri->node1() == n1) && (tri->node2() == n2) && (tri->node3() == n3)){
				return tri;
			}
			if((tri->node1() == n2) && (tri->node2() == n3) && (tri->node3() == n1)){
				return tri;
			}
			if((tri->node1() == n3) && (tri->node2() == n1) && (tri->node3() == n2)){
				return tri;
			}
		}
	}
	return nullptr;
}


bool Graph::connectionExists(QSharedPointer<Node> fromNode,QSharedPointer<Node> toNode, bool directionMatters) const{
	return !connectionFor(fromNode, toNode, directionMatters).isNull();
}


bool Graph::triExists(QSharedPointer<Node> n1, QSharedPointer<Node> n2, QSharedPointer<Node> n3, bool directionMatters) const{
	return !triFor(n1, n2, n3, directionMatters).isNull();
}


QSharedPointer<Node> Graph::nodeByName(const QString &name) const{
	for(auto const &node: mNodes){
		if(node->name()==name){
			return node;
		}
	}
	return nullptr;
}


QSharedPointer<Node> Graph::nodeByID(const QString &id) const{
	for(auto const &node: mNodes){
		if(node->id()==id){
			return node;
		}
	}
	return nullptr;
}


QVector<QSharedPointer<Node> > Graph::nodes() const{
	return  mNodes;
}


QVector<QSharedPointer<Node> > Graph::nodesOfType(const QString &type) const{
	QVector<QSharedPointer<Node> > nodes;
	for(auto &node: mNodes){
		//if(node->processor()->characteristics().typeID == type){
			nodes.push_back(node);
		//}
	}
	return nodes;
}


QVector<QSharedPointer<Connection> > Graph::connections() const{
	return mConnections;
}


static qreal squaredDistanceToSegment(const QPointF &p, const QPointF &v, const QPointF &w) {
	qreal l2 = (v.x() - w.x()) * (v.x() - w.x()) + (v.y() - w.y()) * (v.y() - w.y());
	if (l2 == 0.0) return (p.x() - v.x()) * (p.x() - v.x()) + (p.y() - v.y()) * (p.y() - v.y());
	qreal t = ((p.x() - v.x()) * (w.x() - v.x()) + (p.y() - v.y()) * (w.y() - v.y())) / l2;
	t = std::max(0.0, std::min(1.0, t));
	QPointF projection(v.x() + t * (w.x() - v.x()), v.y() + t * (w.y() - v.y()));
	return (p.x() - projection.x()) * (p.x() - projection.x()) + (p.y() - projection.y()) * (p.y() - projection.y());
}


QSharedPointer<Connection> Graph::connectionAt(const QPointF &pt, const qreal radius) const{
	QSharedPointer<Connection> closestConnection;
	const auto radiusSquared = radius * radius;
	auto minDistSquared = std::numeric_limits<qreal>::max();
	for (const auto &connection : mConnections) {
		double distSquared = squaredDistanceToSegment(pt, connection->from()->pos(), connection->to()->pos());
		if (distSquared < minDistSquared) {
			minDistSquared = distSquared;
			closestConnection = connection;
		}
	}
	if(minDistSquared > radiusSquared){
		return nullptr;
	}
	return closestConnection;
}


QSharedPointer<Tri> Graph::triAt(const QPointF &pt) const {
	QSharedPointer<Tri> closestTri;
	double minDistSquared = std::numeric_limits<double>::max();
	
	for (const auto &tri : mTris) {
		// Get triangle vertices
		const auto pos1 = tri->node1()->pos();
		const auto pos2 = tri->node2()->pos();
		const auto pos3 = tri->node3()->pos();
		
		// Calculate centroid of the triangle
		QPointF centroid = (pos1 + pos2 + pos3) / 3.0;
		
		// Check if the point is inside the triangle using barycentric coordinates
		QPointF v0 = pos2 - pos1;
		QPointF v1 = pos3 - pos1;
		QPointF v2 = pt - pos1;
		
		// Compute dot products
		double dot00 = QPointF::dotProduct(v0, v0);
		double dot01 = QPointF::dotProduct(v0, v1);
		double dot02 = QPointF::dotProduct(v0, v2);
		double dot11 = QPointF::dotProduct(v1, v1);
		double dot12 = QPointF::dotProduct(v1, v2);
		
		// Compute barycentric coordinates
		double denom = dot00 * dot11 - dot01 * dot01;
		if (qFuzzyIsNull(denom)) {
			// Degenerate triangle, skip
			continue;
		}
		double invDenom = 1.0 / denom;
		double u = (dot11 * dot02 - dot01 * dot12) * invDenom;
		double v = (dot00 * dot12 - dot01 * dot02) * invDenom;
		
		// Check if point is inside the triangle
		if (u >= 0 && v >= 0 && (u + v) <= 1) {
			// Calculate squared distance to the centroid
			double distSquared = std::pow(pt.x() - centroid.x(), 2) + std::pow(pt.y() - centroid.y(), 2);
			
			// Check if this triangle is closer than the current closest one
			if (distSquared < minDistSquared) {
				minDistSquared = distSquared;
				closestTri = tri;
			}
		}
	}
	
	return closestTri;
}


QVector< QSharedPointer<Connection> > Graph::connectionsForNode(QSharedPointer<Node> node, NodeSelector fromSelector, NodeSelector toSelector) const{
	QVector< QSharedPointer<Connection> > out;
	for(auto const &connection:mConnections){
		const auto f = connection->from() == node;
		switch(fromSelector){
			case(MUST_INCLUDE):{if(!f)continue;}break;
			case(MUST_NOT_INCLUDE):{if(f)continue;}break;
			default:break;
		}
		const auto t = connection->to() == node;
		switch(toSelector){
			case(MUST_INCLUDE):{if(!t)continue;}break;
			case(MUST_NOT_INCLUDE):{if(t)continue;}break;
			default:break;
		}
		out << connection;
	}
	return out;
}


QSharedPointer<Node> Graph::nodeAt(const QPointF &pt, NodeSelector inputSelector, NodeSelector outputSelector, QSharedPointer<Node> exclude) const{
	for(auto &node: mNodes){
		if(node->rect().contains(pt)){
			/*
			const auto ti = node->processor()->characteristics().takesAudioInput;
			switch(inputSelector){
				case(MUST_INCLUDE):{if(!ti)continue;}break;
				case(MUST_NOT_INCLUDE):{if(ti)continue;}break;
				default:break;
			}
			const auto go = node->processor()->characteristics().givesAudioOutput;
			switch(outputSelector){
				case(MUST_INCLUDE):{if(!go)continue;}break;
				case(MUST_NOT_INCLUDE):{if(go)continue;}break;
				default:break;
			}
			if(exclude && node == exclude){
				continue;
			}
*/
			return node;
		}
	}
	return nullptr;
}


QVector<QSharedPointer<Node> > Graph::nodesAt(const QRectF &lasso, const LassoType lassoType, QSharedPointer<Node> exclude) const{
	QVector<QSharedPointer<Node> > ret;
	for(auto &node: mNodes){
		bool selected{false};
		switch(lassoType){
		case(Intersection):{
			selected = lasso.intersects(node->rect());
		}break;
		case(CenterIn):{
			selected = lasso.contains(node->pos());
		}break;
		default:
		case(RectIn):{
			selected = lasso.contains(node->rect());
		}break;
		}

		if(selected){
			/*
			const auto ti = node->processor()->characteristics().takesAudioInput;
			switch(inputSelector){
				case(MUST_INCLUDE):{if(!ti)continue;}break;
				case(MUST_NOT_INCLUDE):{if(ti)continue;}break;
				default:break;
			}
			const auto go = node->processor()->characteristics().givesAudioOutput;
			switch(outputSelector){
				case(MUST_INCLUDE):{if(!go)continue;}break;
				case(MUST_NOT_INCLUDE):{if(go)continue;}break;
				default:break;
			}
*/
			if(exclude && node == exclude){
				continue;
			}
			ret << node;
		}
	}
	return ret;
}


void Graph::drawStats(QPainter &painter, const Style &style) const{
	painter.setFont(style.nodeFont);
	painter.setPen(style.connectionPen);
	int selected=0;
	for(const auto &node:mNodes){
		if(node->selected()){
			selected++;
		}
	}
	painter.drawText(QPointF(10, 10), QString("nodes: %1 (%2 selected), connections: %3, tris: %4").arg(mNodes.size()).arg(selected).arg(mConnections.size()).arg(mTris.size()));
}


void Graph::draw(QPainter &painter, const bool xray, const Style &style) const{
	if(mDebug){
		qDebug()<<"drawing graph";
	}
	for(auto &tri:this->mTris){
		if(!tri){
			qWarning()<<"No tri";
			continue;
		}
		tri->draw(painter, style);
	}
	for(auto &connection:this->mConnections){
		if(!connection){
			qWarning()<<"No connection";
			continue;
		}
		connection->draw(painter, style);
	}
	for(auto &node:this->mNodes){
		if(node){
			node->draw(painter, xray, style);
		}
	}
	drawStats(painter, style);
}



QDataStream& operator<<(QDataStream& out, const Graph& obj) {
	const qsizetype nodeSize = obj.mNodes.size();
	qDebug() << "Stream OUT NodeGraph: nodeSize=" << nodeSize;
	out << nodeSize;
	for(auto const &node:obj.mNodes){
		out << *node;
	}
	const qsizetype connectionsSize = obj.mConnections.size();
	qDebug() << "Stream OUT NodeGraph: connectionsSize=" << connectionsSize;
	out << connectionsSize;
	for(auto const &connection:obj.mConnections){
		out << *connection;
	}
	return out;
}


QDataStream& operator>>(QDataStream& in, Graph& obj) {
	/*
	qsizetype nodesSize{0};
	in >> nodesSize;
	qDebug() << "Stream IN NodeGraph: nodesSize=" << nodesSize;
	obj.mNodes.clear();
	obj.mNodes.reserve(nodesSize);
	for(qsizetype i=0;i<nodesSize;++i){
		Node *node = new Node(obj.mGraph, "", "", QPoint());
		in >> *node;
		obj.mNodes.append(QSharedPointer<Node>(node));
	}
	qsizetype connectionSize{0};
	in >> connectionSize;
	qDebug() << "Stream IN NodeGraph: nodesSize=" << connectionSize;
	obj.mConnections.clear();
	obj.mConnections.reserve(connectionSize);
	for(qsizetype i=0;i<connectionSize;++i){
		Connection *connection = new Connection(obj.mGraph, nullptr, nullptr);
		in >> *connection;
		obj.mConnections.append(QSharedPointer<Connection>(connection));
	}
*/
	return in;
}


static QRectF calculateBoundingBox(const QVector<QSharedPointer<Node>> &nodes) {
	if (nodes.isEmpty()){
		return QRectF();
	}
	qreal minX = nodes[0]->pos().x();
	qreal minY = nodes[0]->pos().y();
	qreal maxX = minX;
	qreal maxY = minY;
	for (const auto &node : nodes) {
		QPointF pos = node->pos();
		minX = std::min(minX, pos.x());
		minY = std::min(minY, pos.y());
		maxX = std::max(maxX, pos.x());
		maxY = std::max(maxY, pos.y());
	}
	return QRectF(minX, minY, maxX - minX, maxY - minY);
}


static void adjustNodes(const QRectF &target, const QMarginsF &margins, QVector<QSharedPointer<Node>> &nodes) {
	if (nodes.isEmpty()){
		return;
	}
	QRectF boundingBox = calculateBoundingBox(nodes);
	QRectF adjustedTarget = target.marginsRemoved(margins);
	qreal scaleX = adjustedTarget.width() / boundingBox.width();
	qreal scaleY = adjustedTarget.height() / boundingBox.height();
	qreal scale = std::min(scaleX, scaleY);
	qreal offsetX = adjustedTarget.left() + (adjustedTarget.width() - boundingBox.width() * scale) / 2.0;
	qreal offsetY = adjustedTarget.top() + (adjustedTarget.height() - boundingBox.height() * scale) / 2.0;
	for (const auto &node : nodes) {
		QPointF pos = node->pos();
		qreal newX = (pos.x() - boundingBox.left()) * scale + offsetX;
		qreal newY = (pos.y() - boundingBox.top()) * scale + offsetY;
		node->setPos(QPointF(newX, newY));
	}
}

void Graph::zoomToFit(const QRectF &target){
	if(mNodes.isEmpty()){
		return;
	}
	const qreal m{100.0};
	adjustNodes(target, QMarginsF(m, m, m, m), mNodes);
}



static void place(QPointF &point, QSharedPointer<Node> node){
	auto size = node->size();
	node->setPos(QPointF(point.x(), point.y() * size.height()));
	auto pos = node->pos();
	point.setX(pos.x()+size.width());
}


void Graph::autoArrange(const QRectF &boundary) {
	// Constants for the force-directed algorithm
	static const int forcePasses{200};
	static const int adjustPasses{15};
	static const double iterationSize{0.15};
	static const double idealNeighbourDistanceFactor{5.0};
	static const double idealConnectionDistanceFactor{idealNeighbourDistanceFactor*0.3};
	QVector<QVector2D> forces(mNodes.size(), QVector2D(0, 0));
	const bool doNeighbourRepel{true};
	const bool doConnectionAttract{true};
	const bool doCompact{true};
	for (int adjustIteration = 0; adjustIteration < adjustPasses; ++adjustIteration) {
		for (int forceIteration = 0; forceIteration < forcePasses; ++forceIteration) {
			for (int i = 0; i < mNodes.size(); ++i) {
				forces[i]=QVector2D(0, 0);
			}
			if(doNeighbourRepel){
				for (int i = 0; i < mNodes.size(); ++i) {
					for (int j = i + 1; j < mNodes.size(); ++j) {
						QVector2D delta = QVector2D(mNodes[i]->pos()) - QVector2D(mNodes[j]->pos());
						double distance = delta.length() + 0.0001;
						const auto idealDist = ((mNodes[i]->size().width() + mNodes[j]->size().width())*idealNeighbourDistanceFactor) / 2.0;
						double forceMagnitude = 0.0;
						if (distance < idealDist) {
							forceMagnitude = (idealDist - distance) / idealDist;
						} else {
							forceMagnitude = -std::exp(-(distance - idealDist));
						}
						QVector2D forceDirection = delta.normalized();
						QVector2D force = forceDirection * forceMagnitude;
						forces[i] += force;
						forces[j] -= force;
					}
				}
			}
			if(doConnectionAttract){
				for (const auto &connection : mConnections) {
					int i = mNodes.indexOf(connection->from());
					int j = mNodes.indexOf(connection->to());
					if(i == j){
						qWarning()<<"Identical to and from index for connection" << i;
						continue;
					}
					if(i < 0){
						qWarning()<<"Zero from index for connection" << i;
						continue;
					}
					if(j < 0){
						qWarning()<<"Zero to index for connection" << j;
						continue;
					}
					QVector2D delta = QVector2D(mNodes[i]->pos()) - QVector2D(mNodes[j]->pos());
					const auto distance = delta.length() + 0.0001;
					const auto idealDist = ((mNodes[i]->size().width() + mNodes[j]->size().width())*idealConnectionDistanceFactor) / 2.0;
					const auto diff = idealDist-distance;
					const QVector2D force = diff * delta.normalized();
					forces[i] += force;
					forces[j] -= force;
				}
			}
			if(doCompact){
				const auto center=boundary.center();
				for (int i = 0; i < mNodes.size(); ++i) {
					const auto node = mNodes[i];
					QPointF pos = node->pos();
					const QVector2D force(center-pos);
					forces[i] += force*0.001;
				}
			}
			for (int i = 0; i < mNodes.size(); ++i) {
				QVector2D newPos = QVector2D(mNodes[i]->pos()) + forces[i] * iterationSize;
				double halfWidth = mNodes[i]->size().width() / 2.0;
				double halfHeight = mNodes[i]->size().height() / 2.0;
				newPos.setX(qBound(boundary.left() + halfWidth, newPos.x(), boundary.right() - halfWidth));
				newPos.setY(qBound(boundary.top() + halfHeight, newPos.y(), boundary.bottom() - halfHeight));
				mNodes[i]->setPos(newPos.toPointF());
			}
		}
		const qreal m{100.0};
		adjustNodes(boundary, QMarginsF(m, m, m, m), mNodes);
	}
}



bool saveGraph(QSharedPointer<Graph> graph, const QString &filename, const bool compress){
	qDebug() << "Saving "<<filename;
	if(!graph){
		qWarning()<<"No Graph to save, skipping...";
		return false;
	}
	QFile file(filename);
	if (!file.open(QIODevice::WriteOnly)) {
		qWarning()<<"Could not open file for saving, skipping...";
		return false;
	}
	QDataStream out(&file);
	out.writeRawData(MAGIC.toUtf8().constData(), 4);
	out << VERSION;
	out << compress;
	if (compress) {
		QByteArray data;
		QDataStream dataStream(&data, QIODevice::WriteOnly);
		dataStream << *graph;
		QByteArray compressedData = qCompress(data);
		out << compressedData;
	} else {
		out << *graph;
	}
	file.close();
	graph->unTaint("saveGraph");
	return true;
}


bool loadGraph(QSharedPointer<Graph> graph, const QString &filename){
	qDebug() << "Loading "<<filename;
	if(!graph){
		qWarning()<<"No Graph to load, skipping...";
		return false;
	}
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly)) {
		qWarning()<<"Could not open file for loading, skipping...";
		return false;
	}
	QDataStream in(&file);
	char magic[5] = {0};
	in.readRawData(magic, 4);
	if (QString::fromUtf8(magic) != MAGIC) {
		file.close();
		qWarning()<<"Wrong MAGIC found in file while loading, skipping...";
		return false;
	}
	quint32 version;
	in >> version;
	if (version != VERSION) {
		file.close();
		qWarning()<<"Wrong VERSION found in file while loading, skipping...";
		return false;
	}
	bool compress{false};
	in >> compress;
	if (compress) {
		QByteArray compressedData;
		in >> compressedData; 
		QByteArray data = qUncompress(compressedData);
		QDataStream dataStream(&data, QIODevice::ReadOnly);
		dataStream >> *graph;
	} else {
		in >> *graph;
	}
	graph->unTaint("loadGraph");
	return true;
}

