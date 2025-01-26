#include "NodeView.hpp"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QImage>
#include <QMarginsF>
#include <QMessageBox>
#include <QMouseEvent>
#include <QSettings>
#include <QStandardPaths>
#include <QTimer>
#include <QWindow>

#include "Connection.hpp"
#include "GeometryEngine.hpp"
#include "Graph.hpp"
#include "Node.hpp"
#include "Project.hpp"
#include "RenameDialog.hpp"
#include "Settings.hpp"
#include "Tri.hpp"
#include "d3/nodes/Player.hpp"
#include "ui/ConnectionParameterWidget.hpp"
#include "ui/NodeParameterWidget.hpp"
#include "ui/TriParameterWidget.hpp"

static const QString SETTINGS_KEY_LAST_FILE_DIR{"node_view_last_file_dir"};
static const QString FILE_ENDING{".nv"};
static const QString fileFilter{"node-view files (*"+FILE_ENDING+");;All Files (*.*)"};

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
	loadSettings();
	updateSelectNodeMenu();
}


void NodeView::addNodeToSelectNodeMenu(QString type){
	if(!mProject){
		if(mDebug){
			qDebug()<<"Projectnt"<<type;
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
	if(!mProject){
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
	if(!mProject){
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
			mProject->graph()->removeNode(node);
			update();
		});
	}
}


void NodeView::updateConnectionContextMenu(QSharedPointer<Connection> connection){
	mConnectionContextMenu->clear();
	if(!mProject){
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
			mProject->graph()->removeConnection(connection);
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
	if(!mProject){
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
			mProject->graph()->removeTri(tri);
			update();
		});
	}
}

void NodeView::onStep(qreal dt){
	Q_UNUSED(dt);
	update();
}


void NodeView::setProject(QSharedPointer<Project> project){
	if(mDebug){
		qDebug()<<"Set project to" << project << "(Node view)";
	}
	mProject = project;
	connect(mProject->player().data(), &Player::step, this, &NodeView::onStep);
	update();
	if(!mProject){
		return;
	}
	updateSelectNodeMenu();
}

QSharedPointer<Project> NodeView::project(){
	return mProject;
}

void NodeView::setSettings(QSharedPointer<QSettings> settings){
	this->mSettings=settings;
	if(this->mSettings){
		//auto outputDevice=mSettings->value(SETTINGS_KEY_AUDIO_OUTPUT_DEVICE, QString()).toString();
	}
}



void NodeView::zoomToFit(const QRectF &target){
	auto graph = mProject;
	if(!graph){
		return;
	}
	mProject->graph()->zoomToFit(target);
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
	if(!mProject){
		return;
	}
	nodeCt++;
	auto node = QSharedPointer<Node>::create(mProject, QString("%2").arg(nodeCt), pos);
	mProject->graph()->addNode(node);
	update();
}






void NodeView::paintEvent(QPaintEvent *){
	QPainter painter(this);
	antialiasHints(painter);
	if(! mProject){
		return;
	}
	if(! mProject){
		return;
	}
	const auto style = *mProject->style();
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
	mProject->graph()->draw(painter, mXray, style);
	painter.restore();
}




void NodeView::invalidateLinesToNode(QSharedPointer<Node> node){
	auto connections = mProject->graph()->connectionsForNode(node);
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
	auto nodes = mProject->graph()->nodes();
	mSelection.clear();
	for(const auto &node: nodes){
		const auto selected = !node->isSelected();
		if(selected){
			mSelection.append(node);
		}
		node->setSelected(selected);
	}
	update();
}


bool NodeView::flip(const QSharedPointer<Node> &n1, const QSharedPointer<Node> &n2, const QSharedPointer<Node> &n3, const QSharedPointer<Node> &n4){
	auto graph = mProject->graph();
	const auto a1 = graph->triFor(n1, n2, n4, false);
	const auto a2 = graph->triFor(n2, n3, n4, false);
	const auto b1 = graph->triFor(n1, n3, n4, false);
	const auto b2 = graph->triFor(n1, n2, n3, false);
	
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
		return false;
	}
	if(a){
		graph->removeTri(a1);
		graph->removeTri(a2);
		graph->addTri(n1, n2, n3);
		graph->addTri(n1, n3, n4);
	}
	else{
		graph->removeTri(b1);
		graph->removeTri(b2);
		graph->addTri(n2, n3, n4);
		graph->addTri(n2, n4, n1);
	}
	update();
	return true;
}

bool NodeView::flipSelection(){
	if(mSelection.size() != 4){
		qWarning() << "Cannot flip without exactly 4 nodes selected";
		return false;
	}
	const auto n1 = mSelection[0];
	const auto n2 = mSelection[1];
	const auto n3 = mSelection[2];
	const auto n4 = mSelection[3];
	return flip(n1, n2, n3, n4) || flip(n1, n2, n4, n3);
}


bool NodeView::relax(){
	if(mSelection.size() <= 0){
		qWarning() << "Cannot relax without any nodes selected";
		return false;
	}
	GeometryEngine ge;
	ge.relax(mProject->graph(), mSelection);
	update();
	return true;
}


void NodeView::interConnectSelected(){
	GeometryEngine ge;
	ge.interConnect(mProject->graph(), mSelection);
	update();
}



void NodeView::interTriSelected() {
	GeometryEngine ge;
	ge.interTri(mProject->graph(), mSelection);
	update();
}



void NodeView::removeSelected(){
	if(!mSelection.empty()){
		for(const auto &node: mSelection){
			mProject->graph()->removeNode(node);
		}
		mSelection.clear();
		update();
	}
}

void NodeView::toggleRunning(){
	mProject->player()->setRunning(!mProject->player()->isRunning());
	update();
}


void NodeView::loadSettings(){
	mSettings = Settings::loadSettings();
}




bool NodeView::clear(){
	mProject->graph()->clear();
	return true;
}


void NodeView::save(){
	QString suggestedName;
	auto lastPath = mSettings->value(SETTINGS_KEY_LAST_FILE_DIR, QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)).toString();
	QFileInfo lastPathInfo(lastPath);
	QString directory = lastPathInfo.absolutePath();
	QString filename = lastPathInfo.fileName();
	if (!suggestedName.isEmpty()) {
		filename = suggestedName;
	}
	QDir dir(directory);
	const auto suggestedFilePath = dir.filePath(filename);
	QString filePath = QFileDialog::getSaveFileName(this, "Save File", suggestedFilePath, fileFilter);
	if(mDebug){
		qDebug() << "SAVE FILES suggested=" << suggestedFilePath << " actual=" << filePath;
	}
	if (!filePath.isEmpty()) {
		if (!filePath.endsWith(FILE_ENDING, Qt::CaseInsensitive)) {
			filePath += FILE_ENDING;
		}
		saveProject(mProject, filePath);
		mSettings->setValue(SETTINGS_KEY_LAST_FILE_DIR, filePath);
	}
}

void NodeView::load(){
	QString suggestedName;
	if(!clear()){
		return;
	}
	auto lastPath = mSettings->value(SETTINGS_KEY_LAST_FILE_DIR, QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)).toString();
	QFileInfo lastPathInfo(lastPath);
	QString directory = lastPathInfo.absolutePath();
	QString filename = lastPathInfo.fileName();
	if (!suggestedName.isEmpty()) {
		filename = suggestedName;
	}
	QDir dir(directory);
	const auto suggestedFilePath = dir.filePath(filename);
	QString filePath = QFileDialog::getOpenFileName(this, "Open File", suggestedFilePath, fileFilter);
	if(mDebug){
		qDebug() << "LOAD FILES suggested=" << suggestedFilePath << " actual=" << filePath;
	}
	if (!filePath.isEmpty()) {
		loadProject(mProject, filePath);
		mSettings->setValue(SETTINGS_KEY_LAST_FILE_DIR, filePath);
	}
	update();
}


void NodeView::mousePressEvent(QMouseEvent *event){
	if(mDebug){
		qDebug()<<"mouse press" << event;
	}
	if(!mProject){
		qWarning()<<"no graph";
		return;
	}
	const auto but = event->button();
	const auto pos = event->pos();
	if(but == Qt::RightButton){
		const auto &node = mProject->graph()->nodeAt(pos);
		if(node){
			updateNodeContextMenu(node);
			const auto popup_pos = QCursor::pos() - QPoint(mNodeContextMenu->width(), mNodeContextMenu->height())/2;
			mNodeContextMenu->popup(popup_pos);
		}
		else{
			const auto connection = mProject->graph()->connectionAt(pos);
			if(connection){
				updateConnectionContextMenu(connection);
				const auto popup_pos = QCursor::pos() - QPoint(mConnectionContextMenu->width(), mConnectionContextMenu->height())/2;
				mConnectionContextMenu->popup(popup_pos);
			}
			else{
				const auto tri = mProject->graph()->triAt(pos);
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
		if(! mProject){
			return;
		}
		mDragStartPos = pos;
		mDragLastPos = pos;
		mDragEndPos = pos;
		if(event->modifiers() & Qt::ShiftModifier){
			auto node = mProject->graph()->nodeAt(pos);
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
			mSourceNode = mProject->graph()->nodeAt(pos);
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
	if(!mProject){
		return;
	}
	const auto pos = event->pos();
	switch(dragMode){
		default:
		case(OFF):{}break;
		case(CONNECT_NODE):{
			const auto nodes = mProject->graph();
			if(! nodes){
				return;
			}
			if(mSourceNode){
				if(mDestinationNode){
					mDestinationNode->deSelect();
				}
				mDestinationNode = nodes->nodeAt(pos);
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
			mSelection = mProject->graph()->nodesAt(lasso);
			syncSelection(true);
			update();
		}break;
	}
}


void NodeView::mouseReleaseEvent(QMouseEvent *event){
	if(!mProject){
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
			const auto nodes = mProject->graph();
			if(! nodes){
				return;
			}
			if(mSourceNode){
				if(mDestinationNode){
					mDestinationNode->deSelect();
				}
				mDestinationNode = nodes->nodeAt(pos);
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
			const auto graph = mProject->graph();
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
		const auto clickedNode = mProject->graph()->nodeAt(pos);
		QSharedPointer<QWidget> view{nullptr};
		if(clickedNode){
			auto nodeView = nodeParameterWidget(clickedNode);
			view = nodeView;
		}
		else{
			const auto clickedConnection = mProject->graph()->connectionAt(pos);
			if(clickedConnection){
				auto connectionView = connectionParameterWidget(clickedConnection);
				view = connectionView;
			}
			else{
				const auto clickedTri= mProject->graph()->triAt(pos);
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
	auto nodes = mProject->graph()->nodes();
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
	if (event->key() == Qt::Key_A) {
		event->accept();
		zoomToFit(rect());
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
	else if (event->key() == Qt::Key_R) {
		event->accept();
		relax();
	}
	else if (event->key() == Qt::Key_S) {
		event->accept();
		save();
	}
	else if (event->key() == Qt::Key_L) {
		event->accept();
		load();
	}
	else if (event->key() == Qt::Key_Space) {
		event->accept();
		toggleRunning();
	}
	else if (event->key() == Qt::Key_1) {
		event->accept();
		auto lastPath = mSettings->value(SETTINGS_KEY_LAST_FILE_DIR, QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)).toString();
		loadProject(mProject, lastPath);
		update();
	}
	else if (event->key() == Qt::Key_2) {
		event->accept();
		auto lastPath = mSettings->value(SETTINGS_KEY_LAST_FILE_DIR, QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)).toString();
		saveProject(mProject, lastPath);
		update();
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
