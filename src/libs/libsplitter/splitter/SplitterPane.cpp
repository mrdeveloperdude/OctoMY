#include "SplitterPane.hpp"
#include "ui_SplitterPane.h"

#include "command/CommandWidget.hpp"
#include "splitter/SplitterWidget.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QDrag>
#include <QEvent>
#include <QMenu>
#include <QMimeData>
#include <QMouseEvent>
#include <QRandomGenerator>
#include <QShortcut>
#include <QSplitter>
#include <QStyle>



/*
2025-01-11

TODO: List of changes for splitter

 [V] Convert to ui form class
 [V] Move controls to top bar per pane
 [V] Minimalize controls
 [V] Disable splitter collapse
 [V] Add arrange control
 [V] Implement visual focus indication
 [V] Implement drag and drop of panes to other locations
 [V] Implement visual drop indication
 [V] Add keyboard shortcuts for navigating and propegating panes
 [ ] Closing last pane should do something
 [ ] Create mvp command interface widget and integrate with splitter
 [ ] Create URI namespace decoupling framework
 [ ] Save structure into "constellations"
 [ ] Add support for multiple windows
 [ ] Add support for multiple monitors
*/

// This line won hack of the year 2024
#define UndecidedOrientation (static_cast<Qt::Orientation>(0x3))


QString EdgeToString(const Edge type) {
	switch (type) {
#define EdgeToStringStanza(A) case(Edge::A): return QStringLiteral(#A); break
		EdgeToStringStanza(Left);
		EdgeToStringStanza(Right);
		EdgeToStringStanza(Top);
		EdgeToStringStanza(Bottom);
		EdgeToStringStanza(None);
#undef EdgeToStringStanza
	}
	return "UNKNOWN";
}



QDebug operator<<(QDebug debug, const Edge &type) {
	debug.nospace() << EdgeToString(type);
	return debug.space();
}



static int n{0};

static const auto DRAG_MIME{"application/x-splitterpane-source"};

static void replaceWidgetInSplitter(QSplitter *splitter, QWidget *oldWidget, QWidget *newWidget) {
	OC_FUNCTIONGATE();
	const auto index = splitter->indexOf(oldWidget);
	if (index == -1) {
		qWarning() << "Widget not found in the splitter.";
		return;
	}
	const auto sizes = splitter->sizes();
	splitter->replaceWidget(index, newWidget);
	splitter->setSizes(sizes);
}


static void addAfterWidgetInSplitter(QSplitter *splitter, QWidget *existingWidget, QWidget *newWidget) {
	OC_FUNCTIONGATE();
	const auto index = splitter->indexOf(existingWidget);
	if (index == -1 || existingWidget == nullptr) {
		splitter->addWidget(newWidget);
		return;
	}
	// Keep size disturbance to a minimum
	auto sizes = splitter->sizes();
	auto existingWidgetSize = sizes[index];
	splitter->insertWidget(index + 1, newWidget);
	sizes[index] = existingWidgetSize / 2;
	sizes.insert(index + 1, existingWidgetSize / 2);
	splitter->setSizes(sizes);
}


static void setSplitterEqualSizes(QSplitter *splitter) {
	OC_FUNCTIONGATE();
	const auto count = splitter->count();
	if (count == 0){
		return;
	}
	splitter->setSizes(QList<int> (count, splitter->size().width() / count));
}


static Edge closestEdge(const QPoint &mousePos, const QRect &rect, bool allowVertical=true, bool allowHorizontal=true) {
	OC_FUNCTIONGATE();
	const auto TOO_FAR{9999999999};
	const auto leftDist = allowHorizontal?std::abs(mousePos.x() - rect.left()):TOO_FAR;
	const auto rightDist = allowHorizontal?std::abs(mousePos.x() - rect.right()):TOO_FAR;
	const auto topDist = allowVertical?std::abs(mousePos.y() - rect.top()):TOO_FAR;
	const auto bottomDist = allowVertical?std::abs(mousePos.y() - rect.bottom()):TOO_FAR;
	const auto minDist = std::min({leftDist, rightDist, topDist, bottomDist, TOO_FAR-1});
	if (minDist == leftDist) {
		return Left;
	} else if (minDist == rightDist) {
		return Right;
	} else if (minDist == topDist) {
		return Top;
	} else if (minDist == bottomDist) {
		return Bottom;
	}
	else{
		return None;
	}
}


////////////////////////////////////////////////////////////////////////////////


SplitterPane::SplitterPane(
	  QWidget *parent
	, SplitterWidget *splitterWidget
	)
	: QWidget(parent)
	, ui(new Ui::SplitterPane)
	, splitterWidget(splitterWidget)
	, mMenu(new QMenu(this))
{
	OC_METHODGATE();
	ui->setupUi(this);
	ui->stackedWidgetContent->setCurrentWidget(ui->pageMenu);
	auto name = QString("Pane %1").arg(n++);
	ui->labelTitle->setText(name);
	auto color = QColor::fromHsv(((n*59*(360*6))/60)%360, ((n*599*(100*3))/600)%100+155, ((n*5999*(100*2))/6000)%100+155);
	ui->labelTitle->setStyleSheet(QString("background-color: %1; color:black;").arg(color.name()));
	setObjectName(name.replace(" ", "-"));
	//ui->labelTitle->setStyleSheet(QString("#%1 {background-color: %2; }").arg(objectName()).arg(color.name()));
	updateFocusVisual();
	this->installEventFilter(this);
	if(nullptr != splitterWidget){
		connect(this, &SplitterPane::paneRequestFocus, splitterWidget, &SplitterWidget::setFocusedPane);
	}
	setAcceptDrops(true);
	buildMenu();
}


SplitterPane::~SplitterPane()
{
	OC_METHODGATE();
	delete ui;
	if(splitterWidget && splitterWidget->focusedPane() == this){
		splitterWidget->setFocusedPane(nullptr);
	}
}


SplitterPane *SplitterPane::splitPane(Qt::Orientation orientation) {
	OC_METHODGATE();
	auto pSplitter = parentSplitter();
	if(!pSplitter){
		qWarning() << "Could not find parent splitter";
		return nullptr;
	}
	auto newPane = OC_NEW SplitterPane(nullptr, splitterWidget);
	if(!newPane){
		qWarning() << "Could not create new editor pane";
		return nullptr;
	}
	qDebug()<<"Splitting " << objectName() << orientation;
	if(pSplitter->count() <= 1){
		pSplitter->setOrientation(orientation);
	}
	if (pSplitter->orientation() == orientation) {
		addAfterWidgetInSplitter(pSplitter, this, newPane);
	} else {
		auto replacementSplitter = new QSplitter(orientation, this);
		if(!replacementSplitter){
			qWarning() << "Could not create replacement splitter";
			newPane->deleteLater();
			return nullptr;
		}
		replacementSplitter->setChildrenCollapsible(false);
		replaceWidgetInSplitter(pSplitter, this, replacementSplitter);
		replacementSplitter->addWidget(this);
		replacementSplitter->addWidget(newPane);
	}
	return newPane;
}


SplitterPane *SplitterPane::splitPaneVertically(){
	OC_METHODGATE();
	return splitPane(Qt::Vertical);
}


SplitterPane *SplitterPane::splitPaneHorizontally(){
	OC_METHODGATE();
	return splitPane(Qt::Horizontal);
}


void SplitterPane::arrangeSplitter(){
	OC_METHODGATE();
	auto pSplitter = parentSplitter();
	if (!pSplitter) {
		qWarning() << "Could not find parent splitter";
		return;
	}
	qDebug()<<"Arranging " << objectName();
	setSplitterEqualSizes(pSplitter);
}


SplitterPane* SplitterPane::detachPane() {
	OC_METHODGATE();
	qDebug() << "Detaching " << objectName();
	auto pSplitter = parentSplitter();
	if (!pSplitter) {
		qWarning() << "Could not find parent splitter";
		return nullptr;
	}
	SplitterPane *replacement{nullptr};
	const auto count = pSplitter->count();
	if (count == 1) {
		// If this is the only pane in the splitter, handle splitter removal
		auto parentSplitter = qobject_cast<QSplitter*>(pSplitter->parentWidget());
		if (parentSplitter) {
			replacement = qobject_cast<SplitterPane *>(pSplitter->widget(0));
			if (replacement) {
				replaceWidgetInSplitter(parentSplitter, pSplitter, replacement);
			}
			pSplitter->deleteLater();
		} else {
			// Top-level splitter; just delete it
			pSplitter->deleteLater();
		}
	} else {
		// If there are multiple panes, just remove this one
		const auto index = pSplitter->indexOf(this);
		if (index != -1) {
			if (index >= count - 1) {
				replacement = qobject_cast<SplitterPane *>(pSplitter->widget(index - 1));
			} else {
				replacement = qobject_cast<SplitterPane *>(pSplitter->widget(index + 1));
			}
		}
	}
	
	// Disconnect this pane from the splitter
	this->setParent(nullptr);
	
	// Return a replacement pane (if applicable)
	return replacement;
}


void SplitterPane::closePane() {
	OC_METHODGATE();
	qDebug()<<"Closing " << objectName();
	auto replacement = detachPane();
	deleteLater();
	emit paneRequestFocus(replacement);
}


bool SplitterPane::movePane(SplitterPane *targetPane, Edge targetDropEdge) {
	OC_METHODGATE();
	SplitterPane *sourcePane{this};
	if (None == targetDropEdge) {
		qWarning() << "No target drop edge";
		return false;
	}
	auto sourceSplitter = sourcePane->parentSplitter();
	if (!sourceSplitter) {
		qWarning() << "Source splitter not found.";
		return false;
	}
	auto targetSplitter = targetPane->parentSplitter();
	if (!targetSplitter) {
		qWarning() << "Target splitter not found.";
		return false;
	}
	qDebug()<<"Moving " << sourcePane->objectName() << " -->" << targetPane->objectName() << targetDropEdge;
	sourcePane->detachPane();
	int targetIndex = targetSplitter->indexOf(targetPane);
	switch(targetSplitter->orientation()){
		default:
		case (Qt::Horizontal):{
			if (targetDropEdge == Left) {
				targetSplitter->insertWidget(targetIndex, sourcePane);
			} else if (targetDropEdge == Right) {
				if (targetIndex == targetSplitter->count() - 1) {
					targetSplitter->addWidget(sourcePane);
				} else {
					targetSplitter->insertWidget(targetIndex + 1, sourcePane);
				}
			}
			else{
				qWarning()<<"Invalid target drop edge for horizontal orientation" << targetDropEdge;
				return false;
			}
		} break;
		case (Qt::Vertical) :{
			if (targetDropEdge == Top) {
				targetSplitter->insertWidget(targetIndex, sourcePane);
			} else if (targetDropEdge == Bottom) {
				if (targetIndex == targetSplitter->count() - 1) {
					targetSplitter->addWidget(sourcePane);
				} else {
					targetSplitter->insertWidget(targetIndex + 1, sourcePane);
				}
			}
			else{
				qWarning()<<"Invalid target drop edge for vertical orientation" << targetDropEdge;
				return false;
			}
		}break;
	}
	if (sourceSplitter->count() <= 0) {
		qWarning()<<"Cleaning up splitters. Should not be necessary";
		sourceSplitter->deleteLater();
	}
	return true;
}


void SplitterPane::setPaneFocused(bool focused){
	OC_METHODGATE();
	mPanelFocused = focused;
	mEdge = None;
	updateFocusVisual();
}


void SplitterPane::setFrameStyle(const QString &style){
	OC_METHODGATE();
	if(ui && ui->frame){
		const auto s = QString("#%1 QFrame#frame {%2}").arg(objectName()).arg(style);
		// qDebug()<<"Setting style: "<< s;
		ui->frame->setStyleSheet(s);
	}
}


void SplitterPane::updateFocusVisual() {
	OC_METHODGATE();
	QString style;
	if (mPanelFocused) {
		style = "border: 3px solid palette(Highlight);";
	} else {
		style = "border: 3px dotted transparent;";
	}
	switch(mEdge){
	case(Left): style += "border-left-color: palette(Highlight);"; break;
	case(Right): style += "border-right-color: palette(Highlight);"; break;
	case(Top): style += "border-top-color: palette(Highlight);"; break;
	case(Bottom): style += "border-bottom-color: palette(Highlight);"; break;
	case None:
		break;
	}
	setFrameStyle(style);
}


void SplitterPane::updateDropIndicator(const QPoint &mousePos) {
	OC_METHODGATE();
	QRect rect = ui->frame->geometry();
	mEdge = None;
	const auto pSplitter = parentSplitter();
	if(pSplitter){
		const auto orientation = pSplitter->orientation();
		mEdge = closestEdge(mousePos, rect, orientation==Qt::Vertical, orientation==Qt::Horizontal);
	}
	updateFocusVisual();
}


void SplitterPane::setContentWidget(QWidget *newWidget) {
	OC_METHODGATE();
	if (!ui->pageContent) {
		qWarning() << "pageContent is not initialized.";
		return;
	}
	auto layout = ui->pageContent->layout();
	if (!layout) {
		layout = new QVBoxLayout(ui->pageContent);
		layout->setContentsMargins(0, 0, 0, 0);
		ui->pageContent->setLayout(layout);
	}
	while (auto child = layout->takeAt(0)) {
		if (child->widget()) {
			child->widget()->deleteLater();
		}
		delete child;
	}
	if (newWidget) {
		layout->addWidget(newWidget);
		ui->stackedWidgetContent->setCurrentWidget(ui->pageContent);
		newWidget->setFocus();
	}
	else{
		ui->stackedWidgetContent->setCurrentWidget(ui->pageMenu);
	}
}


void SplitterPane::buildMenu(){
	OC_METHODGATE();
	mMenu->clear();
	mMenu->addSeparator()->setText(QString("Tools"));
	{
		QAction *cliAction = mMenu->addAction("&Commandline");
		QObject::connect(cliAction, &QAction::triggered, this, [=](){
			auto cli = OC_NEW CommandWidget();
			setContentWidget(cli);
			//update();
		});
	}
}


void SplitterPane::showMenu(){
	OC_METHODGATE();
	const auto popup_pos = QCursor::pos() - QPoint(mMenu->width(), mMenu->height())/2;
	mMenu->popup(popup_pos);
}


void SplitterPane::startDrag(){
	OC_METHODGATE();
	qDebug()<<"Drag started from" << objectName();
	auto drag = new QDrag(this);
	auto mimeData = new QMimeData;
	//mimeData->setText(ui->labelTitle->text());
	mimeData->setData(DRAG_MIME, this->objectName().toUtf8());
	drag->setMimeData(mimeData);
	drag->setPixmap(ui->labelTitle->grab());
	this->setEnabled(false);
	const auto result = drag->exec(Qt::MoveAction);
	this->setEnabled(true);
	if (result == Qt::MoveAction) {
		qDebug() << "Drop was accepted with action:" << result;
	} else {
		qDebug() << "Drop was not accepted with action:" << result;
	}
}


void SplitterPane::dragEnterEvent(QDragEnterEvent *event) {
	OC_METHODGATE();
	if (event->mimeData()->hasFormat(DRAG_MIME) && Qt::MoveAction == event->proposedAction()) {
		event->acceptProposedAction();
		updateDropIndicator(event->position().toPoint());
	} else {
		event->ignore();
	}
}


void SplitterPane::dragMoveEvent(QDragMoveEvent *event) {
	OC_METHODGATE();
	updateDropIndicator(event->position().toPoint());
	event->acceptProposedAction();
}


void SplitterPane::dragLeaveEvent(QDragLeaveEvent *event) {
	OC_METHODGATE();
	mEdge = None;
	updateFocusVisual();
	event->accept();
}


void SplitterPane::dropEvent(QDropEvent *event) {
	OC_METHODGATE();
	if (event->mimeData()->hasFormat(DRAG_MIME) && Qt::MoveAction == event->proposedAction()) {
		QString sourceName = QString::fromUtf8(event->mimeData()->data("application/x-splitterpane-source"));
		auto sourcePane = splitterWidget->findChild<SplitterPane *>(sourceName);
		if (!sourcePane) {
			qWarning() << "Source pane not found: " << sourceName;
			event->ignore();
			return;
		}
		if (this == sourcePane) {
			qWarning() << "Source and destination pane the same: " << sourceName;
			event->ignore();
			return;
		}
		if (sourcePane->movePane(this, mEdge)) {
			event->acceptProposedAction();
			qDebug() << "Pane moved successfully.";
		} else {
			event->ignore();
			qWarning() << "Invalid drop position.";
		}
	} else {
		event->ignore();
	}
	mEdge = None;
	updateFocusVisual();
}


bool SplitterPane::eventFilter(QObject *watched, QEvent *event) {
	OC_METHODGATE();
	const auto type = event->type();
	if (type == QEvent::MouseButtonPress || type == QEvent::MouseButtonDblClick) {
		QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
		if (this->rect().contains(this->mapFromGlobal(mouseEvent->globalPosition().toPoint()))) {
			emit paneRequestFocus(this);
		}
	}
	return QWidget::eventFilter(watched, event);
}


Qt::Orientation SplitterPane::orientation() const{
	OC_METHODGATE();
	auto pSplitter = parentSplitter();
	if(pSplitter){
		return pSplitter->count()>1? pSplitter->orientation():UndecidedOrientation;
	}
	return UndecidedOrientation;
}


QSplitter* SplitterPane::parentSplitter() const {
	OC_METHODGATE();
	return qobject_cast<QSplitter*>(this->parentWidget());
}


void SplitterPane::mousePressEvent(QMouseEvent *event) {
	OC_METHODGATE();
	if (event->button() == Qt::LeftButton) {
		dragStartPosition = event->pos();
	}
}


void SplitterPane::mouseMoveEvent(QMouseEvent *event) {
	OC_METHODGATE();
	if (event->buttons() & Qt::LeftButton) {
		const auto distance = (event->pos() - dragStartPosition).manhattanLength();
		if (distance < QApplication::startDragDistance()){
			return;
		}
		startDrag();
	}
}

