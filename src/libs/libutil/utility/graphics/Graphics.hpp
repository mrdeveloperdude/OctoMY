#ifndef UTILITY_GRAPHICS_HPP
#define UTILITY_GRAPHICS_HPP

#include <QColor>



class QPainter;
class QRectF;
class QPointF;
class QImage;
class QString;


namespace utility
{

namespace graphics
{

QImage tint(QImage src, QColor color, qreal strength=1.0);
void drawText(QPainter & painter, qreal x, qreal y, Qt::Alignment flags, const QString & text, QRectF * boundingRect = nullptr);
void drawText(QPainter & painter, const QPointF & point, Qt::Alignment flags, const QString & text, QRectF * boundingRect = {});

QRgb randomColor( );
QImage randomImage(int w, int h);

QByteArray imageToByteArray(QImage &image );
QByteArray randomImageByteArray(int w, int h);
QByteArray randomJPEGByteArray(int w, int h, int q);

}
}


#endif
// UTILITY_GRAPHICS_HPP
