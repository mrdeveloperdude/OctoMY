#ifndef UTILS_H
#define UTILS_H

#include <QImage>
#include <QDebug>
#include <rhi/qrhi.h>

class QTextStream;
class QMatrix4x4;

QImage dummyTexture(const QSize textureSize, const QString &text="Some Text");
void updateTexture(QRhiResourceUpdateBatch *resourceUpdates, const QString &text, const QSize textureSize = QSize(64, 64) );
QShader getShader(const QString &name);
void listResources(const QString &path = ":/");
QTextStream &operator<<(QTextStream &stream, const QMatrix4x4 &matrix);
QDebug operator<<(QDebug dbg, const std::array<float, 16> &arr);
#endif // UTILS_H
