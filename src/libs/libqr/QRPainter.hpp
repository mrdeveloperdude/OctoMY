#ifndef QRPAINTER_HPP
#define QRPAINTER_HPP

#include <QPainter>

void paintQR(QPainter &painter, const QSize sz, const QString &data, QColor fg=Qt::black);

#endif // QRPAINTER_HPP
