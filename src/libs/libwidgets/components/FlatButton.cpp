#include "FlatButton.hpp"

#include "uptime/MethodGate.hpp"

#include <QBitmap>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionButton>

FlatButton::FlatButton(QWidget *parent)
	: QPushButton(parent)
{
	OC_METHODGATE();
}


FlatButton::~FlatButton()
{
	OC_METHODGATE();
}

void FlatButton::paintEvent(QPaintEvent *event)
{
	OC_METHODGATE();
	Q_UNUSED(event);
	QIcon ico = icon();
	if (ico.isNull()){
		return;
	}

	const QPalette &pal = palette();
	QColor iconColor;
	if (!isEnabled()) {
		iconColor = pal.color(QPalette::Disabled,  QPalette::ButtonText);
	}
	else if (isDown() || isChecked()) {
		iconColor = pal.color(QPalette::Active,    QPalette::Highlight);
	}
	else if (underMouse()) {
		iconColor = pal.color(QPalette::Active,    QPalette::Midlight);
	}
	else {
		iconColor = pal.color(QPalette::Active,    QPalette::ButtonText);
	}
	QStyleOptionButton opt;
	initStyleOption(&opt);
	QRect contentRect = style()->subElementRect(QStyle::SE_PushButtonContents, &opt, this);
	int side = qMin(contentRect.width(), contentRect.height());
	QSize iconSize(side, side);
	qreal dpr = devicePixelRatioF();
	QPixmap pix = ico.pixmap(iconSize * dpr, isEnabled() ? (isDown() ? QIcon::Active : QIcon::Normal) : QIcon::Disabled, isChecked() ? QIcon::On : QIcon::Off);
	pix.setDevicePixelRatio(dpr);
	QPixmap tinted(pix.size());
	tinted.fill(Qt::transparent);
	{
		QPainter tp(&tinted);
		tp.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing);
		tp.drawPixmap(0, 0, pix);
		tp.setCompositionMode(QPainter::CompositionMode_SourceIn);
		tp.fillRect(tinted.rect(), iconColor);
	}
	QSize finalSize = tinted.size() / dpr;
	int x = contentRect.x() + (contentRect.width()  - finalSize.width())  / 2;
	int y = contentRect.y() + (contentRect.height() - finalSize.height()) / 2;
	QPainter painter(this);
	painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing);
	painter.drawPixmap(x, y, tinted);
}
