#include "Graph.hpp"

#include "Connection.hpp"
#include "Node.hpp"
#include "Project.hpp"
#include "Tri.hpp"
#include "d3/nodes/Player.hpp"

#include <QDebug>
#include <QFile>
#include <QIODevice>
#include <QRandomGenerator>
#include <QSharedPointer>
#include <QVector2D>


Graph::Graph(QSharedPointer<Project> project)
	: mProject(project)
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
	mProject->taint(reason);
}

bool Graph::clear(){
	mConnections.clear();
	mTris.clear();
	mNodes.clear();
	mTopologyChanged = true;
	taint("clear()");
	return true;
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


QSharedPointer<Tri> Graph::triFor(const QSharedPointer<Node> &n1, const QSharedPointer<Node> &n2, const QSharedPointer<Node> &n3, bool orderMatters, bool directionMatters) const{
	for(auto const &tri:mTris){
		const auto &t1{tri->node1()};
		const auto &t2{tri->node2()};
		const auto &t3{tri->node3()};
		if((t1 == n1) && (t2 == n2) && (t3 == n3)){
			return tri;
		}
		if(!orderMatters) {
			if((t1 == n2) && (t2 == n3) && (t3 == n1)){
				return tri;
			}
			if((t1 == n3) && (t2 == n1) && (t3 == n2)){
				return tri;
			}
		}
	}
	if(!directionMatters){
		// Do it again with alternate direction
		return triFor(n1, n3, n2, orderMatters, true);
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
	Q_UNUSED(type);
	// TODO: Implement
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


QSharedPointer<Node> Graph::nodeAt(const QPointF &pt) const{
	for(auto &node: mNodes){
		if(node->rect().contains(pt)){
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
		if(node->isSelected()){
			selected++;
		}
	}
	painter.drawText(QPointF(10, 10), QString("nodes: %1 (%2 selected), connections: %3, tris: %4, running: %5").arg(mNodes.size()).arg(selected).arg(mConnections.size()).arg(mTris.size()).arg(mProject->player()->isRunning()?"TRUE":"FALSE"));
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



void Graph::step(qreal dt) {
	for (auto &c : mConnections) {
		auto n1 = c->from();
		auto n2 = c->to();
		auto diff = n1->pos() - n2->pos();
		auto diff2d = QVector3D(diff);
		auto currentLength = diff2d.length();
		auto direction = diff2d.normalized();
		auto rl = c->restLength();
		auto k = c->stiffness();
		auto displacement = currentLength - rl;
		auto springForce = -k * displacement * direction;
		auto b = c->damping();
		auto relativeVelocity = n1->velocity() - n2->velocity();
		auto dampingForce = -b * QVector3D::dotProduct(relativeVelocity, direction) * direction;
		auto totalForce = springForce + dampingForce;
		n1->applyForce(totalForce);
		n2->applyForce(-totalForce);
	}
	for (auto &n : mNodes) {
		n->integrate(dt);
	}
}





QDataStream& operator<<(QDataStream& out, const Graph& obj) {
	{
		const qsizetype nodeSize = obj.mNodes.size();
		qDebug() << "Stream OUT Graph: nodes=" << nodeSize;
		out << nodeSize;
		for(auto const &node:obj.mNodes){
			out << *node;
		}
	}
	{
		const qsizetype connectionsSize = obj.mConnections.size();
		qDebug() << "Stream OUT Graph: connections=" << connectionsSize;
		out << connectionsSize;
		for(auto const &connection:obj.mConnections){
			out << *connection;
		}
	}
	{
		const qsizetype triSize = obj.mTris.size();
		qDebug() << "Stream OUT Graph: tris=" << triSize;
		out << triSize;
		for(auto const &tri:obj.mTris){
			out << *tri;
		}
	}
	return out;
}


QDataStream& operator>>(QDataStream& in, Graph& obj) {
	{
		qsizetype nodesSize{0};
		in >> nodesSize;
		qDebug() << "Stream IN Graph: nodes=" << nodesSize;
		obj.mNodes.clear();
		obj.mNodes.reserve(nodesSize);
		for(qsizetype i=0;i<nodesSize;++i){
			auto node = QSharedPointer<Node>::create(obj.project());
			in >> *node;
			obj.mNodes.append(node);
		}
	}
	{
		qsizetype connectionSize{0};
		in >> connectionSize;
		qDebug() << "Stream IN Graph: connections=" << connectionSize;
		obj.mConnections.clear();
		obj.mConnections.reserve(connectionSize);
		for(qsizetype i=0;i<connectionSize;++i){
			auto connection = QSharedPointer<Connection>::create(obj.sharedFromThis());
			in >> *connection;
			obj.mConnections.append(connection);
		}
	}
	{
		qsizetype triSize{0};
		in >> triSize;
		qDebug() << "Stream IN Graph: tris=" << triSize;
		obj.mTris.clear();
		obj.mTris.reserve(triSize);
		for(qsizetype i=0;i<triSize;++i){
			auto tri = QSharedPointer<Tri>::create(obj.sharedFromThis());
			in >> *tri;
			obj.mTris.append(tri);
		}
	}
	
	return in;
}

