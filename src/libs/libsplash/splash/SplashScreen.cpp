#include "SplashScreen.hpp"
#include "ui_SplashScreen.h"
#include "PulsatingLabel.hpp"

#include <QEvent>
#include <QGraphicsOpacityEffect>
#include <QPalette>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QResizeEvent>

SplashScreen::SplashScreen(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::SplashScreen)
	, pulseAnimation(nullptr)
	, progressPercentage(0)
	, opacityEffect(new QGraphicsOpacityEffect(this))
	, fadeOut(new QPropertyAnimation(opacityEffect, "opacity"))
{
	ui->setupUi(this);
	
	// Now that ui->labelImage is valid (and promoted to PulsatingLabel), create a sequential animation group for pulsation.
	{
		auto group = new QSequentialAnimationGroup(this);
		// Forward animation: scale from 0.8 to 1.2 over 500ms.
		forwardAnim = new QPropertyAnimation(ui->labelImage, "scaleFactor");
		forwardAnim->setDuration(500);
		forwardAnim->setStartValue(0.8);
		forwardAnim->setEndValue(1.0);
		forwardAnim->setEasingCurve(QEasingCurve::InOutQuad);
		
		// Reverse animation: scale from 1.2 back to 0.8 over 500ms.
		backwardAnim = new QPropertyAnimation(ui->labelImage, "scaleFactor");
		backwardAnim->setDuration(500);
		backwardAnim->setStartValue(1.0);
		backwardAnim->setEndValue(0.8);
		backwardAnim->setEasingCurve(QEasingCurve::InOutQuad);
		
		group->addAnimation(forwardAnim);
		group->addAnimation(backwardAnim);
		group->setLoopCount(-1); // Loop indefinitely.
		
		pulseAnimation = group;
	}
	
	// Set the splash screen to be opaque over its parent.
	setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
	setAttribute(Qt::WA_TranslucentBackground, false);
	setAutoFillBackground(true);
	QPalette pal = palette();
	QColor bgColor = pal.color(QPalette::Window);
	bgColor.setAlpha(255); // Ensure full opacity.
	pal.setColor(QPalette::Window, bgColor);
	setPalette(pal);
	
	// Hide the progress bar initially.
	ui->progressBar->setVisible(false);
	ui->labelMessage->setText("");
	
	// Set up fade-out animation using the opacity effect.
	opacityEffect->setOpacity(1.0);
	setGraphicsEffect(opacityEffect);
	fadeOut->setStartValue(1.0);
	fadeOut->setEndValue(0.0);
	connect(fadeOut, &QPropertyAnimation::finished, [this]() {
		hide();
		pulseAnimation->stop();
		fadeOut->deleteLater();
		if (parentWidget()){
			parentWidget()->removeEventFilter(this);
		}
		deleteLater();
	});
}

SplashScreen::~SplashScreen() {
	delete ui;
}

void SplashScreen::configure(QWidget *parent, const QString &imagePath, const quint16 pulseTime, const quint16 fadeTime, const qreal lowSize) {
	if (parent) {
		// Make this widget a child of the given parent and cover its entire area.
		setParent(parent);
		setGeometry(parent->rect());
		// Update geometry on parent resize.
		parent->installEventFilter(this);
	}
	ui->labelImage->setSvg(imagePath);
	fadeOut->setDuration(fadeTime);
	forwardAnim->setStartValue(lowSize);
	backwardAnim->setEndValue(lowSize);
	forwardAnim->setDuration(pulseTime/2);
	backwardAnim->setDuration(pulseTime/2);
	pulseAnimation->start();
	show();
}

void SplashScreen::setProgressPercentage(quint8 percentage) {
	progressPercentage = percentage;
	ui->progressBar->setValue(percentage);
	ui->progressBar->setVisible(percentage > 0);
	emit progressUpdated(percentage);
}

void SplashScreen::setMessage(const QString &message) {
	ui->labelMessage->setText(message);
}

void SplashScreen::done() {
	// Start fade-out animation.
	fadeOut->start();
}

bool SplashScreen::eventFilter(QObject *watched, QEvent *event) {
	// When the parent is resized, update the splash screen's geometry.
	if (watched == parentWidget() && event->type() == QEvent::Resize) {
		setGeometry(parentWidget()->rect());
	}
	return QWidget::eventFilter(watched, event);
}
