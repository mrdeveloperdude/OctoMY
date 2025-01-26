#ifndef FORMATUTILS_HPP
#define FORMATUTILS_HPP

#include <QImage>
#include <QString>
#include <QVideoFrameFormat>

QString imageFormatToString(QImage::Format format);
QString videoFormatToString(QVideoFrameFormat::PixelFormat format);

#endif // FORMATUTILS_HPP
