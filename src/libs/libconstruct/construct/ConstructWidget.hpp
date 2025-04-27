#ifndef CONSTRUCTWIDGET_HPP
#define CONSTRUCTWIDGET_HPP

#include <QElapsedTimer>
#include <QRhiWidget>
#include <rhi/qrhi.h>

#include "nodes/Layer.hpp"


class Scene;
class RhiContext;
class InputHandler;

class ConstructWidget : public QRhiWidget
{
	Q_OBJECT
private:
	QSharedPointer<RhiContext> mContext;
	QSharedPointer<Scene> mScene;
	QElapsedTimer mTimer;
	qreal mAngle{0};
	QSet<int> mPressedKeys;
	bool mIsMiddleMousePressed{false};
	QPointF mLastMousePos;
	InputHandler *mInputHandler{nullptr};

public:
	ConstructWidget(QWidget *parent = nullptr);
	~ConstructWidget();

	// QRhiWidget interface
public:
	void initialize(QRhiCommandBuffer *cb) override;
	void render(QRhiCommandBuffer *cb) override;
	void releaseResources() override;

private:
	void createScene();
	void updateMovement();
	void hookInputEvents();

signals:
	void resized();
	void rhiChanged(const QString &apiName);
protected:
	void keyPressEvent(QKeyEvent *event) override;
	void keyReleaseEvent(QKeyEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void wheelEvent(QWheelEvent *event) override;
};

//CONSTRUCTWIDGET_HPP
#endif
