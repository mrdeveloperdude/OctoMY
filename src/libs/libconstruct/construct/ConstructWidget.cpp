#include "ConstructWidget.hpp"

#include <QCoreApplication>
#include <QDebug>
#include <QPainter>
#include <QKeyEvent>
#include <QLoggingCategory>

#include "RhiContext.hpp"
#include "Scene.hpp"
#include "Utils.hpp"
#include "nodes/CubeNode.hpp"
#include "input/InputHandler.hpp"


ConstructWidget::ConstructWidget(QWidget *parent)
	: QRhiWidget(parent)
	, mContext(QSharedPointer<RhiContext>::create())
	, mScene(QSharedPointer<Scene>::create())
	, mInputHandler(new InputHandler(this))
{
	Q_UNUSED(listResources);
	QLoggingCategory::setFilterRules(QStringLiteral("qt.rhi.debug=true"));
	
	setFocusPolicy(Qt::StrongFocus);
	//listResources(":/");
	//qDebug() << "ConstructWidget::ctor";
	mContext->depthStencilClearValue = new QRhiDepthStencilClearValue(1.0, 0.0);
	mTimer.start();
	hookInputEvents();
}

ConstructWidget::~ConstructWidget(){
	delete mInputHandler;
	mInputHandler = nullptr;
}

void ConstructWidget::createScene(){
	//qDebug() << "createScene";
	mScene->clear();
	mScene->addGroundplane(*mContext);
	mScene->addShape(*mContext);
	
	mScene->addCube(*mContext);
	mScene->addCube(*mContext);
	
	
	/*
	mScene->addTexTri(*mContext);
	mScene->addTri(*mContext);
	mScene->addCube("Cube 2", 45.0f);
	mScene->addCube("Cube 3", 90.0f);
	mScene->addCube(*mContext, "The Cube", 69.42f);
*/

}


void ConstructWidget::hookInputEvents(){
	//qDebug()<<"HOOK";
	// In ConstructWidget constructor or initialization:
	connect(mInputHandler, &InputHandler::dollyTriggered, this, [this](qreal delta) {
		qDebug()<<"DT";
		if (auto camera = mScene->camera()) {
			// You might apply a sensitivity factor here.
			camera->dolly(delta);
		}
	});
	
	connect(mInputHandler, &InputHandler::panTriggered, this, [this](const QPointF &delta) {
		qDebug()<<"PT";
		if (auto camera = mScene->camera()) {
			// Adjust for desired pan speed.
			camera->pan(delta.x(), delta.y());
		}
	});
	
	connect(mInputHandler, &InputHandler::rotateTriggered, this, [this](const QPointF &delta) {
		qDebug()<<"RT";
		if (auto camera = mScene->camera()) {
			// Adjust for desired rotation speed.
			camera->lookAround(delta.x(), delta.y());
		}
	});
	
	connect(mInputHandler, &InputHandler::orbitTriggered, this, [this](const QPointF &delta) {
		qDebug()<<"RO";
		if (auto camera = mScene->camera()) {
			// Adjust for desired rotation speed.
			camera->orbit(delta.x(), delta.y());
		}
	});
	
	
	connect(mInputHandler, &InputHandler::resetViewTriggered, this, [this]() {
		qDebug()<<"RV";
		if (auto camera = mScene->camera()) {
			camera->reset();
		}
	});
}

void ConstructWidget::initialize(QRhiCommandBuffer *cb)
{
	//qDebug() << "QRhiWidget::initialize()";
	mContext->cb = cb;
	auto wrhi = rhi();
	if (mContext->m_rhi != wrhi) {
		mContext->m_rhi = wrhi;
		mContext->mDirty = true;
		auto backendName = QString::fromUtf8(mContext->m_rhi->backendName());
		//qDebug() << "New RHI with backend:" << backendName;
		emit rhiChanged(backendName);
	}
	mContext->resourceUpdates = mContext->m_rhi->nextResourceUpdateBatch();
	mContext->renderTarget = renderTarget();
	mContext->renderPassDescriptor = mContext->renderTarget->renderPassDescriptor();
	if (mContext->m_pixelSize != mContext->renderTarget->pixelSize()) {
		mContext->m_pixelSize = mContext->renderTarget->pixelSize();
		emit resized();
	}
	
	auto sampleCount = mContext->renderTarget->sampleCount();
	if (mContext->m_sampleCount != sampleCount) {
		//qDebug() << "New sample count:" << mContext->m_sampleCount << "-->" << sampleCount;
		mContext->m_sampleCount = sampleCount;
		mContext->mDirty = true;
	}
	if(mContext->mDirty){
		createScene();
		mContext->mDirty = false;
	}
	mContext->updateViewMatrix();
	mScene->initialize(*mContext);
}


void ConstructWidget::render(QRhiCommandBuffer *cb)
{
	updateMovement();
	mAngle=fmod(mAngle+0.123, M_PI*2);
	//qDebug() << "QRhiWidget::render()";
	mContext->cb = cb;
	mContext->renderTarget = renderTarget();
	mContext->renderPassDescriptor = mContext->renderTarget->renderPassDescriptor();
	mContext->resourceUpdates = mContext->m_rhi->nextResourceUpdateBatch();
	mScene->update(*mContext, mTimer.restart());
	if(nullptr!=mContext->resourceUpdates){
		mContext->cb->resourceUpdate(mContext->resourceUpdates);
		mContext->resourceUpdates = nullptr;
	}
	mContext->cb->beginPass(mContext->renderTarget, mScene->clearColor(), *mContext->depthStencilClearValue);
	
	const auto outputSize = mContext->renderTarget->pixelSize();
	mContext->cb->setViewport(QRhiViewport(0, 0, outputSize.width(), outputSize.height()));

	mScene->render(*mContext);
	mContext->cb->endPass();
	update();
}


void ConstructWidget::releaseResources()
{
	//qDebug() << "QRhiWidget::releaseResources()";
	mScene->releaseResources();
}

void ConstructWidget::keyPressEvent(QKeyEvent *event)
{
	mInputHandler->handleKeyPress(event);
}

void ConstructWidget::keyReleaseEvent(QKeyEvent *event)
{
	mInputHandler->handleKeyRelease(event);
}

void ConstructWidget::mousePressEvent(QMouseEvent *event)
{
	mInputHandler->handleMousePress(event);
}

void ConstructWidget::mouseReleaseEvent(QMouseEvent *event)
{
	mInputHandler->handleMouseRelease(event);
}

void ConstructWidget::mouseMoveEvent(QMouseEvent *event)
{
	mInputHandler->handleMouseMove(event);
}

void ConstructWidget::wheelEvent(QWheelEvent *event)
{
	mInputHandler->handleMouseWheelEvent(event);
}

void ConstructWidget::updateMovement()
{
	mInputHandler->processInput();

}

