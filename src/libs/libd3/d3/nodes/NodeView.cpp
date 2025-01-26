#include "NodeView.hpp"

#include <QApplication>
#include <QDebug>
#include <QImage>
#include <QMarginsF>
#include <QMessageBox>
#include <QMouseEvent>
#include <QSettings>
#include <QTimer>
#include <QWindow>

#include "Connection.hpp"
#include "GeometryEngine.hpp"
#include "Graph.hpp"
#include "Node.hpp"
#include "RenameDialog.hpp"
#include "Tri.hpp"
#include "ui/ConnectionParameterWidget.hpp"
#include "ui/NodeParameterWidget.hpp"
#include "ui/TriParameterWidget.hpp"


void antialiasHints(QPainter &painter){
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setRenderHint(QPainter::SmoothPixmapTransform);
	painter.setRenderHint(QPainter::SmoothPixmapTransform , true);
	painter.setRenderHint(QPainter::TextAntialiasing);
	painter.setRenderHint(QPainter::TextAntialiasing , true);
}



NodeView::NodeView(QWidget *parent)
	: QWidget{parent}
	, mSelectNodeMenu(new QMenu(this))
	, mNodeContextMenu(new QMenu(this))
	, mConnectionContextMenu(new QMenu(this))
	, mTriContextMenu(new QMenu(this))
	, renameDialogue(new RenameDialog(this))
{
	this->setMouseTracking(true);
	updateSelectNodeMenu();
}


void NodeView::addNodeToSelectNodeMenu(QString type){
	if(!mGraph){
		if(mDebug){
			qDebug()<<"Graphnt"<<type;
		}
		return;
	}
	QAction *action = mSelectNodeMenu->addAction(type);
	if(mDebug){
		qDebug()<<"Adding" << type;
	}
	QObject::connect(action, &QAction::triggered, this, [=](){
		this->addNode(mapFromGlobal(QCursor::pos()), type);
	});
}


void NodeView::updateSelectNodeMenu(){
	mSelectNodeMenu->clear();
	mSelectNodeMenu->addSeparator()->setText(tr("Add Node"));
	if(!mGraph){
		return;
	}
	QStringList names{"Node"};
	if(mDebug){
		qDebug()<<"Names"<<names;
	}
	for(const auto &name:names){
		addNodeToSelectNodeMenu(name);
	}
}


void NodeView::updateNodeContextMenu(QSharedPointer<Node> node){
	mNodeContextMenu->clear();
	if(!mGraph){
		return;
	}
	if(!node){
		mNodeContextMenu->addSeparator()->setText("Node Unknown");
		return;
	}
	mNodeContextMenu->addSeparator()->setText(QString("Node %1").arg(node->name()));
	{
		QAction *renameAction = mNodeContextMenu->addAction("&Rename...");
		QObject::connect(renameAction, &QAction::triggered, this, [=](){
			auto originalName = node->name();
			renameDialogue->setName(originalName);
			if (renameDialogue->exec() == QDialog::Accepted) {
				auto changedName = renameDialogue->name();
				qDebug() << "Changed name from:" << originalName << " --> " << changedName;
				node->setName(changedName);
				update();
			}
			
		});
		
	}
	{
		QAction *deleteAction = mNodeContextMenu->addAction("&Delete");
		QObject::connect(deleteAction, &QAction::triggered, this, [=](){
			mGraph->removeNode(node);
			update();
		});
	}
}


void NodeView::updateConnectionContextMenu(QSharedPointer<Connection> connection){
	mConnectionContextMenu->clear();
	if(!mGraph){
		return;
	}
	if(!connection){
		mConnectionContextMenu->addSeparator()->setText("Connection Unknown");
		return;
	}
	mConnectionContextMenu->addSeparator()->setText(QStringLiteral("Connection %1 → %2 (%3)").arg(connection->from()->name(), connection->to()->name()));
	{
		QAction *deleteAction = mConnectionContextMenu->addAction("&Delete");
		QObject::connect(deleteAction, &QAction::triggered, this, [=](){
			mGraph->removeConnection(connection);
			update();
		});
	}
	{
		QAction *relaxAction = mConnectionContextMenu->addAction("&Relax");
		QObject::connect(relaxAction, &QAction::triggered, this, [=](){
			connection->relax();
			update();
		});
	}
}

void NodeView::updateTriContextMenu(QSharedPointer<Tri> tri){
	mTriContextMenu->clear();
	if(!mGraph){
		return;
	}
	if(!tri){
		mTriContextMenu->addSeparator()->setText("Tri Unknown");
		return;
	}
	mTriContextMenu->addSeparator()->setText(QStringLiteral("Tri <%1, %2, %3>").arg(tri->node1()->name(), tri->node2()->name(), tri->node3()->name()));
	{
		QAction *deleteAction = mTriContextMenu->addAction("&Delete");
		QObject::connect(deleteAction, &QAction::triggered, this, [=](){
			mGraph->removeTri(tri);
			update();
		});
	}
}


void NodeView::setGraph(QSharedPointer<Graph> graph){
	if(mDebug){
		qDebug()<<"Set graph to" << graph << "(Nodes)";
	}
	mGraph = graph;
	update();
	if(!mGraph){
		return;
	}
	updateSelectNodeMenu();
}

QSharedPointer<Graph> NodeView::graph(){
	return mGraph;
}

void NodeView::setSettings(QSharedPointer<QSettings> settings){
	this->mSettings=settings;
	if(this->mSettings){
		//auto outputDevice=mSettings->value(SETTINGS_KEY_AUDIO_OUTPUT_DEVICE, QString()).toString();
	}
}



void NodeView::zoomToFit(const QRectF &target){
	auto graph = mGraph;
	if(!graph){
		return;
	}
	graph->zoomToFit(target);
	update();
}


void NodeView::autoArrange(const QRectF &target){
	auto graph = mGraph;
	if(!graph){
		return;
	}
	graph->autoArrange(target);
	//updateSolver();
	update();
}



void NodeView::toggleXRay(bool xray_on){
	mXray = xray_on;
	update();
}

static int nodeCt{0};

void NodeView::addNode(QPointF pos, QString type){
	if(mDebug){
		qDebug()<<"ADD "<<type<<"@"<<pos;
	}
	if(!mGraph){
		return;
	}
	nodeCt++;
	auto node = QSharedPointer<Node>::create(mGraph, QString("%2").arg(nodeCt), pos);
	mGraph->addNode(node);
	update();
}






void NodeView::paintEvent(QPaintEvent *){
	QPainter painter(this);
	antialiasHints(painter);
	if(! mGraph){
		return;
	}
	if(! mGraph){
		return;
	}
	const auto style = *mGraph->style();
	painter.fillRect(rect(), style.graphBackground);
	switch(dragMode){
		default:
		case(OFF):{}break;
		case(CONNECT_NODE):{
			painter.setPen(style.connectionPen);
			painter.drawLine(mDragStartPos, mDragEndPos);
		}break;
		case(MOVE_NODE):{
		}break;
		case(PAN_VIEW):{
		}break;
		case(LASSO):{
			const QRectF lassoRect(mDragStartPos, mDragEndPos);
			painter.fillRect(lassoRect, style.lassoBrush);
			painter.setPen(style.lassoPen);
			painter.drawRect(QRectF(mDragStartPos, mDragEndPos));
		}break;
	}
	painter.save();
	painter.translate(mNodeViewPan);
	mGraph->draw(painter, mXray, style);
	painter.restore();
}




void NodeView::invalidateLinesToNode(QSharedPointer<Node> node){
	auto connections = mGraph->connectionsForNode(node);
	for(auto &connection: connections){
		connection->taintLine();
	}
}

void NodeView::syncSelection(bool selected){
	for(const auto &node: mSelection){
		node->setSelected(selected);
	}
}

void NodeView::invertSelection(){
	auto nodes = mGraph->nodes();
	mSelection.clear();
	for(const auto &node: nodes){
		const auto selected = !node->selected();
		if(selected){
			mSelection.append(node);
		}
		node->setSelected(selected);
	}
	update();
}


void NodeView::flipSelection(){
	if(mSelection.size() != 4){
		qWarning() << "Cannot flip without exactly 4 nodes selected";
		return;
	}
	auto n1 = mSelection[0];
	auto n2 = mSelection[1];
	auto n3 = mSelection[2];
	auto n4 = mSelection[3];
	const auto a1 = mGraph->triFor(n1, n2, n3, false);
	const auto a2 = mGraph->triFor(n1, n3, n4, false);
	const auto b1 = mGraph->triFor(n2, n3, n4, false);
	const auto b2 = mGraph->triFor(n2, n4, n1, false);
	const auto a = ((!a1.isNull()) && (!a2.isNull()));
	const auto b = ((!b1.isNull()) && (!b2.isNull()));
	qDebug()<<"N1"<<n1->name();
	qDebug()<<"N2"<<n2->name();
	qDebug()<<"N3"<<n3->name();
	qDebug()<<"N4"<<n4->name();
	
	qDebug()<<"A1"<<(a1.isNull()?"NIL": a1->name());
	qDebug()<<"A2"<<(a2.isNull()?"NIL": a2->name());
	
	qDebug()<<"B1"<<(b1.isNull()?"NIL": b1->name());
	qDebug()<<"B2"<<(b2.isNull()?"NIL": b2->name());
	
	qDebug()<<"A"<<a;
	qDebug()<<"B"<<b;
	
	if(a == b){
		if(a){
			qWarning() << "Cannot flip, too many triangles";
		}
		else{
			qWarning() << "Cannot flip, not enough triangles";
		}
		return;
	}
	if(a){
		mGraph->removeTri(a1);
		mGraph->removeTri(a2);
		mGraph->addTri(n2, n3, n4);
		mGraph->addTri(n2, n4, n1);
	}
	else{
		mGraph->removeTri(b1);
		mGraph->removeTri(b2);
		mGraph->addTri(n1, n2, n3);
		mGraph->addTri(n1, n3, n4);
	}
	update();
}





void NodeView::interConnectSelected(){
	GeometryEngine ge;
	ge.interConnect(mGraph, mSelection);
	update();
}



void NodeView::interTriSelected() {
	GeometryEngine ge;
	ge.interTri(mGraph, mSelection);
	update();
}



void NodeView::removeSelected(){
	if(!mSelection.empty()){
		for(const auto &node: mSelection){
			mGraph->removeNode(node);
		}
		mSelection.clear();
		update();
	}
}



void NodeView::mousePressEvent(QMouseEvent *event){
	if(mDebug){
		qDebug()<<"mouse press" << event;
	}
	if(!mGraph){
		qWarning()<<"no graph";
		return;
	}
	const auto but = event->button();
	const auto pos = event->pos();
	if(but == Qt::RightButton){
		const auto &node = mGraph->nodeAt(pos);
		if(node){
			updateNodeContextMenu(node);
			const auto popup_pos = QCursor::pos() - QPoint(mNodeContextMenu->width(), mNodeContextMenu->height())/2;
			mNodeContextMenu->popup(popup_pos);
		}
		else{
			const auto connection = mGraph->connectionAt(pos);
			if(connection){
				updateConnectionContextMenu(connection);
				const auto popup_pos = QCursor::pos() - QPoint(mConnectionContextMenu->width(), mConnectionContextMenu->height())/2;
				mConnectionContextMenu->popup(popup_pos);
			}
			else{
				const auto tri = mGraph->triAt(pos);
				if(tri){
					updateTriContextMenu(tri);
					const auto popup_pos = QCursor::pos() - QPoint(mTriContextMenu->width(), mTriContextMenu->height())/2;
					mTriContextMenu->popup(popup_pos);
				}
				else{
					const auto popup_pos = QCursor::pos() - QPoint(mSelectNodeMenu->width(), mSelectNodeMenu->height())/2;
					mSelectNodeMenu->popup(popup_pos);
				}
			}
		}
	}
	else if(but == Qt::LeftButton){
		if(! mGraph){
			return;
		}
		mDragStartPos = pos;
		mDragLastPos = pos;
		mDragEndPos = pos;
		if(event->modifiers() & Qt::ShiftModifier){
			auto node = mGraph->nodeAt(pos, EITHER_IS_FINE, MUST_INCLUDE, nullptr);
			mSourceNode = node;
			mDestinationNode = nullptr;
			if(mSourceNode){
				mSourceNode->deSelect();
				dragMode = CONNECT_NODE;
				if(mDebug){
					qDebug()<<"Starting connect" << mDragEndPos;
				}
				update();
			}
		}
		else{
			mSourceNode = mGraph->nodeAt(pos);
			if(mSourceNode){
				if (!mSelection.contains(mSourceNode) ){
					syncSelection(false);
					mSelection.clear();
					mSelection << mSourceNode;
					syncSelection(true);
				}
				dragMode = MOVE_NODE;
				mDragOffsetPos = mSourceNode->rect().center() - pos;
				if(mDebug){
					qDebug()<<"Starting node move" << mDragStartPos;
				}
				update();
			}
			else{
				dragMode = LASSO;
				syncSelection(false);
				mSelection.clear();
				if(mDebug){
					qDebug()<<"Starting lasso" << mDragStartPos;
				}
				update();
			}
		}
	}
	else if(but == Qt::MiddleButton){
		dragMode = PAN_VIEW;
		mDragStartPos = pos;
	}
}



void NodeView::mouseMoveEvent(QMouseEvent *event){
	if(!mGraph){
		return;
	}
	const auto pos = event->pos();
	switch(dragMode){
		default:
		case(OFF):{}break;
		case(CONNECT_NODE):{
			const auto nodes = mGraph;
			if(! nodes){
				return;
			}
			if(mSourceNode){
				if(mDestinationNode){
					mDestinationNode->deSelect();
				}
				mDestinationNode = nodes->nodeAt(pos, MUST_INCLUDE, EITHER_IS_FINE, mSourceNode);
				if(mDestinationNode){
					mDestinationNode->select();
				}
				mDragEndPos = pos;
				//qDebug()<<"connecting"<<mDragEndPos;
				update();
			}
		}break;
		case(MOVE_NODE):{
			mDragEndPos = pos;
			if(!mSelection.isEmpty()){
				for(const auto &node: mSelection){
					invalidateLinesToNode(node);
					node->setPos( node->pos() - (mDragLastPos - mDragEndPos) );
				}
			}
			mDragLastPos = pos;
			//qDebug()<<"moving"<<mDragEndPos;
			update();
		}break;
		case(PAN_VIEW):{
			mDragEndPos = pos;
			mNodeViewPan = mDragEndPos - mDragStartPos;
			update();
		}break;
		case(LASSO):{
			mDragEndPos = pos;
			const QRectF lasso(mDragStartPos, mDragEndPos);
			syncSelection(false);
			mSelection = mGraph->nodesAt(lasso);
			syncSelection(true);
			update();
		}break;
	}
}


void NodeView::mouseReleaseEvent(QMouseEvent *event){
	if(!mGraph){
		return;
	}
	//qDebug()<<"mouse release" << event;
	const auto pos = event->position();
	switch(dragMode){
		default:
		case(OFF):{
			if(!mSelection.isEmpty()){
				syncSelection(false);
				mSelection.clear();
			}
		}break;
		case(CONNECT_NODE):{
			const auto nodes = mGraph;
			if(! nodes){
				return;
			}
			if(mSourceNode){
				if(mDestinationNode){
					mDestinationNode->deSelect();
				}
				mDestinationNode = nodes->nodeAt(pos, MUST_INCLUDE, EITHER_IS_FINE, mSourceNode);
				if(mDestinationNode){
					mDestinationNode->deSelect();
				}
				if(mSourceNode){
					mSourceNode->deSelect();
					if(mDestinationNode){
						if(mDebug){
							qDebug()<<"Making connection";
						}
						const auto status = nodes->addConnection(mSourceNode, mDestinationNode);
						if(!status.ok){
							QMessageBox::warning(this, "Could not connect", status.reason);
						}
					}
				}
				mDragEndPos = pos;
				if(mDebug){
					qDebug()<<"connecting" << mDragEndPos;
					qDebug()<<"ending connect" << mDragEndPos;
				}
				update();
			}
			dragMode=OFF;
		}break;
		case(MOVE_NODE):{
			dragMode=OFF;
			mDragEndPos = pos;
			mSourceNode->setPos( mDragEndPos + mDragOffsetPos );
			if(mDebug){
				qDebug()<<"Ending node move" << mDragEndPos;
			}
			update();
		}break;
		case(PAN_VIEW):{
			dragMode=OFF;
			mDragEndPos = pos;
			mNodeViewPan = mDragEndPos - mDragStartPos;
			const auto graph = mGraph;
			if(! graph){
				return;
			}
			auto nodes = graph->nodes();
			for (auto &node : nodes) {
				node->setPos( node->pos() + mNodeViewPan);
			}
			mNodeViewPan = QPointF();
			update();
		}break;
		case(LASSO):{
			dragMode=OFF;
			mDragEndPos = pos;
			if(mDebug){
				qDebug()<<"Ending lasso" << mDragEndPos;
			}
			update();
		}break;
	}
}


QSharedPointer<NodeParameterWidget> NodeView::nodeParameterWidget(QSharedPointer<Node> node) {
	if(node.isNull()){
		return nullptr;
	}
	
	if(!mNodeParameterWidgets.contains(node)){
		auto nodeView = QSharedPointer<NodeParameterWidget>::create();
		nodeView->setNode(node);
		mNodeParameterWidgets[node] = nodeView;
	}
	return mNodeParameterWidgets[node];
}


QSharedPointer<ConnectionParameterWidget> NodeView::connectionParameterWidget(QSharedPointer<Connection> connection) {
	if(connection.isNull()){
		return nullptr;
	}
	
	if(!mConnectionParameterWidgets.contains(connection)){
		auto connectionView = QSharedPointer<ConnectionParameterWidget>::create();
		connectionView->setConnection(connection);
		mConnectionParameterWidgets[connection] = connectionView;
	}
	return mConnectionParameterWidgets[connection];
}


QSharedPointer<TriParameterWidget> NodeView::triParameterWidget(QSharedPointer<Tri> tri) {
	if(tri.isNull()){
		return nullptr;
	}
	
	if(!mTriParameterWidgets.contains(tri)){
		auto nodeView = QSharedPointer<TriParameterWidget>::create();
		nodeView->setTri(tri);
		mTriParameterWidgets[tri] = nodeView;
	}
	return mTriParameterWidgets[tri];
}


void NodeView::mouseDoubleClickEvent(QMouseEvent *event) {
	if (event->button() == Qt::LeftButton) {
		const auto pos = event->position();
		const auto clickedNode = mGraph->nodeAt(pos);
		QSharedPointer<QWidget> view{nullptr};
		if(clickedNode){
			auto nodeView = nodeParameterWidget(clickedNode);
			view = nodeView;
		}
		else{
			const auto clickedConnection = mGraph->connectionAt(pos);
			if(clickedConnection){
				auto connectionView = connectionParameterWidget(clickedConnection);
				view = connectionView;
			}
			else{
				const auto clickedTri= mGraph->triAt(pos);
				if(clickedTri){
					auto triView = triParameterWidget(clickedTri);
					view = triView;
				}
			}
		}
		if(!view.isNull()){
			const int s=200/(((16+9)/2)*2);
			QPointF sz(s*16, s*9);
			const auto pos = event->globalPosition().toPoint();
			view->setGeometry(QRectF(pos - sz, pos + sz).toAlignedRect());
			view->setWindowFlag(Qt::WindowStaysOnTopHint);
			
			view->show();
			view->setWindowState(Qt::WindowState::WindowActive);
			view->activateWindow();
			view->raise();
			// Flash the window's taskbar entry
			QTimer::singleShot(0, [view] {
				QApplication::alert(view.data(), 2000);
			});
		}
	}
	QWidget::mouseDoubleClickEvent(event);
}


void NodeView::wheelEvent(QWheelEvent *event) {
	const QPointF mousePos = event->position();
	const double zoomFactor = 1.1;
	auto nodes = mGraph->nodes();
	for (auto &node : nodes) {
		QPointF offset = node->pos() - mousePos;
		offset *= (event->angleDelta().y() > 0) ? zoomFactor : 1.0 / zoomFactor;
		node->setPos(mousePos + offset);
	}
	update();
}


void NodeView::closeEvent(QCloseEvent *event) {
	Q_UNUSED(event);
	if(mDebug){
		qDebug()<<"Close event";
	}
}


void NodeView::keyPressEvent(QKeyEvent *event) {
	if (event->key() == Qt::Key_Z) {
		event->accept();
		zoomToFit(rect());
	}
	else if (event->key() == Qt::Key_A) {
		event->accept();
		autoArrange(rect());
	}
	else if (event->key() == Qt::Key_X) {
		event->accept();
		toggleXRay(!mXray);
	}
	else if (event->key() == Qt::Key_N) {
		event->accept();
		this->addNode(mapFromGlobal(QCursor::pos()), "Node");
	}
	else if (event->key() == Qt::Key_C) {
		event->accept();
		interConnectSelected();
	}
	else if (event->key() == Qt::Key_T) {
		event->accept();
		interTriSelected();
	}
	else if (event->key() == Qt::Key_I) {
		event->accept();
		invertSelection();
	}
	else if (event->key() == Qt::Key_F) {
		event->accept();
		flipSelection();
	}
	else if (event->key() == Qt::Key_Delete) {
		event->accept();
		removeSelected();
	}
	else{
		if(mDebug){
			qDebug()<<"Unhandled sequencer KEY" << event->key() << event->modifiers();
		}
		QWidget::keyPressEvent(event);
	}
}
