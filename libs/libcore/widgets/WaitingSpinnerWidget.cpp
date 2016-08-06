#include "WaitingSpinnerWidget.hpp"



/* Original Work Copyright (c) 2012-2014 Alexander Turkin
   Modified 2014 by William Hallatt
   Modified 2015 by Jacob Dawid
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/


#include "basic/Standard.hpp"

// Standard includes
#include <cmath>
#include <algorithm>

// Qt includes
#include <QPainter>
#include <QTimer>

WaitingSpinnerWidget::WaitingSpinnerWidget(QWidget *parent,
										   bool centerOnParent,
										   bool disableParentWhenSpinning)
	: QWidget(parent)
	, mCenterOnParent(centerOnParent)
	, mDisableParentWhenSpinning(disableParentWhenSpinning)
	, mCurrentCounter(0)
	, mIsSpinning(false)
{
	OC_METHODGATE();
	initialize();
}
/*
WaitingSpinnerWidget::WaitingSpinnerWidget(Qt::WindowModality modality,
										   QWidget *parent,
										   bool centerOnParent,
										   bool disableParentWhenSpinning)
	: QWidget(parent, Qt::Dialog | Qt::FramelessWindowHint)
	, mCenterOnParent(centerOnParent)
	, mDisableParentWhenSpinning(disableParentWhenSpinning)
	, mCurrentCounter(0)
	, mIsSpinning(false)
{
	initialize();

	// We need to set the window modality AFTER we've hidden the
	// widget for the first time since changing this property while
	// the widget is visible has no effect.
	setWindowModality(modality);
	setAttribute(Qt::WA_TranslucentBackground);
}
*/
void WaitingSpinnerWidget::initialize()
{
	OC_METHODGATE();
	connect(&mTimer, SIGNAL(timeout()), this, SLOT(rotate()));
	updateSize();
	updateTimer();
	hide();
}

void WaitingSpinnerWidget::paintEvent(QPaintEvent *)
{
	OC_METHODGATE();
	updatePosition();
	QPainter painter(this);
	painter.fillRect(this->rect(), Qt::transparent);
	painter.setRenderHint(QPainter::Antialiasing, true);

	if (mCurrentCounter >= mStyle.mNumberOfLines) {
		mCurrentCounter = 0;
	}

	painter.setPen(Qt::NoPen);
	for (int i = 0; i < mStyle.mNumberOfLines; ++i) {
		painter.save();
		painter.translate(mStyle.mInnerRadius + mStyle.mLineLength,
						  mStyle.mInnerRadius + mStyle.mLineLength);
		qreal rotateAngle =
				static_cast<qreal>(360 * i) / static_cast<qreal>(mStyle.mNumberOfLines);
		painter.rotate(rotateAngle);
		painter.translate(mStyle.mInnerRadius, 0);
		int distance =
				lineCountDistanceFromPrimary(i, mCurrentCounter, mStyle.mNumberOfLines);
		QColor color =
				currentLineColor(distance, mStyle.mNumberOfLines, mStyle.mTrailFadePercentage,
								 mStyle.mMinimumTrailOpacity, mStyle.mColor);
		painter.setBrush(color);
		// TODO improve the way rounded rect is painted
		painter.drawRoundedRect(
					QRect(0, -mStyle.mLineWidth / 2, mStyle.mLineLength,mStyle.mLineWidth), mStyle.mRoundness,
					mStyle.mRoundness, Qt::RelativeSize);
		painter.restore();
	}
	if(!mText.isEmpty()){
		painter.drawText(rect(),mText);
	}
}

void WaitingSpinnerWidget::start()
{
	OC_METHODGATE();
	updatePosition();
	mIsSpinning = true;
	show();

	if(parentWidget() && mDisableParentWhenSpinning) {
		parentWidget()->setEnabled(false);
	}

	if (!mTimer.isActive()) {
		mTimer.start();
		mCurrentCounter = 0;
	}
}

void WaitingSpinnerWidget::stop()
{
	OC_METHODGATE();
	mIsSpinning = false;
	hide();

	if(parentWidget() && mDisableParentWhenSpinning) {
		parentWidget()->setEnabled(true);
	}

	if (mTimer.isActive()) {
		mTimer.stop();
		mCurrentCounter = 0;
	}
}


void WaitingSpinnerWidget::setStarted(bool s)
{
	OC_METHODGATE();
	if(s){
		start();
	}
	else{
		stop();
	}
}

void SpinnerStyle::setColor(QColor color)
{
	OC_METHODGATE();
	mColor = color;
}

void SpinnerStyle::setRoundness(qreal roundness)
{
	OC_METHODGATE();
	mRoundness = qMax(0.0, qMin(1.0, roundness))*100.0;
}

void SpinnerStyle::setMinimumTrailOpacity(qreal minimumTrailOpacity)
{
	OC_METHODGATE();
	mMinimumTrailOpacity = minimumTrailOpacity;
}

void SpinnerStyle::setTrailFadePercentage(qreal trail)
{
	OC_METHODGATE();
	mTrailFadePercentage = trail;
}


void SpinnerStyle::setRevolutionsPerSecond(qreal revolutionsPerSecond)
{
	OC_METHODGATE();
	mRevolutionsPerSecond = revolutionsPerSecond;
}

void SpinnerStyle::setNumberOfLines(int lines)
{
	OC_METHODGATE();
	mNumberOfLines = lines;
}

void SpinnerStyle::setLineLength(qreal length)
{
	OC_METHODGATE();
	mLineLength = length;
}

void SpinnerStyle::setLineWidth(qreal width)
{
	OC_METHODGATE();
	mLineWidth = width;
}

void SpinnerStyle::setInnerRadius(qreal radius)
{
	OC_METHODGATE();
	mInnerRadius = radius;
}

void SpinnerStyle::setRelatveSize(bool rel)
{
	OC_METHODGATE();
	mRelativeSizes = rel;
}

void WaitingSpinnerWidget::setText(QString text)
{
	OC_METHODGATE();
	mText=text;
}

bool WaitingSpinnerWidget::isSpinning() const
{
	OC_METHODGATE();
	return mIsSpinning;
}


void WaitingSpinnerWidget::rotate()
{
	OC_METHODGATE();
	++mCurrentCounter;
	if (mCurrentCounter >= mStyle.mNumberOfLines) {
		mCurrentCounter = 0;
	}
	update();
}


void WaitingSpinnerWidget::updateSize()
{
	OC_METHODGATE();
	int size = (mStyle.mInnerRadius + mStyle.mLineLength) * 2;
	setFixedSize(size, size);
}

void WaitingSpinnerWidget::updateTimer()
{
	OC_METHODGATE();
	mTimer.setInterval(1000 / (mStyle.mNumberOfLines * mStyle.mRevolutionsPerSecond));
}

void WaitingSpinnerWidget::updatePosition()
{
	OC_METHODGATE();
	if (parentWidget() && mCenterOnParent) {
		move(parentWidget()->width() / 2 - width() / 2,
			 parentWidget()->height() / 2 - height() / 2);
	}
}

int WaitingSpinnerWidget::lineCountDistanceFromPrimary(int current, int primary, int totalNrOfLines)
{
	OC_FUNCTIONGATE();
	int distance = primary - current;
	if (distance < 0) {
		distance += totalNrOfLines;
	}
	return distance;
}

QColor WaitingSpinnerWidget::currentLineColor(int countDistance, int totalNrOfLines, qreal trailFadePerc, qreal minOpacity, QColor color)
{
	OC_FUNCTIONGATE();
	if (countDistance == 0) {
		return color;
	}
	const qreal minAlphaF = minOpacity / 100.0;
	int distanceThreshold =
			static_cast<int>(ceil((totalNrOfLines - 1) * trailFadePerc / 100.0));
	if (countDistance > distanceThreshold) {
		color.setAlphaF(minAlphaF);
	} else {
		qreal alphaDiff = color.alphaF() - minAlphaF;
		qreal gradient = alphaDiff / static_cast<qreal>(distanceThreshold + 1);
		qreal resultAlpha = color.alphaF() - gradient * countDistance;

		// If alpha is out of bounds, clip it.
		resultAlpha = std::min(1.0, std::max(0.0, resultAlpha));
		color.setAlphaF(resultAlpha);
	}
	return color;
}
