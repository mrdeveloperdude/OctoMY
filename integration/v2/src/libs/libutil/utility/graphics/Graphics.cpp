#include "Graphics.hpp"

#include "uptime/MethodGate.hpp"


#include <QGraphicsScene>
#include <QGraphicsColorizeEffect>
#include <QGraphicsPixmapItem>

#include <QPainter>

namespace utility
{

namespace graphics
{



QImage tint(QImage src, QColor color, qreal strength)
{
	OC_FUNCTIONGATE();
	if(src.isNull()) {
		return QImage();
	}
	QGraphicsScene scene;
	QGraphicsPixmapItem item;
	item.setPixmap(QPixmap::fromImage(src));
	QGraphicsColorizeEffect effect;
	effect.setColor(color);
	effect.setStrength(strength);
	item.setGraphicsEffect(&effect);
	scene.addItem(&item);
	QImage res(src);
	QPainter ptr(&res);
	scene.render(&ptr, QRectF(), src.rect() );
	return res;
}





void drawText(QPainter & painter, qreal x, qreal y, Qt::Alignment flags, const QString & text, QRectF * boundingRect)
{
	OC_FUNCTIONGATE();
	const qreal size = 32767.0;
	QPointF corner(x, y - size);
	if (flags & Qt::AlignHCenter) {
		corner.rx() -= size/2.0;
	} else if (flags & Qt::AlignRight) {
		corner.rx() -= size;
	}
	if (flags & Qt::AlignVCenter) {
		corner.ry() += size/2.0;
	} else if (flags & Qt::AlignTop) {
		corner.ry() += size;
	} else {
		flags |= Qt::AlignBottom;
	}
	QRectF rect{corner.x(), corner.y(), size, size};
	painter.drawText(rect, static_cast<int>(flags), text, boundingRect);
}

void drawText(QPainter & painter, const QPointF & point, Qt::Alignment flags, const QString & text, QRectF * boundingRect)
{
	OC_FUNCTIONGATE();
	drawText(painter, point.x(), point.y(), flags, text, boundingRect);
}


}
}
