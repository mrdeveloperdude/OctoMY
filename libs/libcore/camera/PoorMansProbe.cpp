#include "PoorMansProbe.hpp"

#include <QVideoFrame>
#include <QCamera>
#include <QCameraViewfinder>


PoorMansProbe::PoorMansProbe(QObject *parent)
	: QAbstractVideoSurface(parent)
	, source(0)
{
}

QList<QVideoFrame::PixelFormat> PoorMansProbe::supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const
{
	Q_UNUSED(handleType);
	return QList<QVideoFrame::PixelFormat>()
		   << QVideoFrame::Format_ARGB32
		   << QVideoFrame::Format_ARGB32_Premultiplied
		   << QVideoFrame::Format_RGB32
		   << QVideoFrame::Format_RGB24
		   << QVideoFrame::Format_RGB565
		   << QVideoFrame::Format_RGB555
		   << QVideoFrame::Format_ARGB8565_Premultiplied
		   << QVideoFrame::Format_BGRA32
		   << QVideoFrame::Format_BGRA32_Premultiplied
		   << QVideoFrame::Format_BGR32
		   << QVideoFrame::Format_BGR24
		   << QVideoFrame::Format_BGR565
		   << QVideoFrame::Format_BGR555
		   << QVideoFrame::Format_BGRA5658_Premultiplied
		   << QVideoFrame::Format_AYUV444
		   << QVideoFrame::Format_AYUV444_Premultiplied
		   << QVideoFrame::Format_YUV444
		   << QVideoFrame::Format_YUV420P
		   << QVideoFrame::Format_YV12
		   << QVideoFrame::Format_UYVY
		   << QVideoFrame::Format_YUYV
		   << QVideoFrame::Format_NV12
		   << QVideoFrame::Format_NV21
		   << QVideoFrame::Format_IMC1
		   << QVideoFrame::Format_IMC2
		   << QVideoFrame::Format_IMC3
		   << QVideoFrame::Format_IMC4
		   << QVideoFrame::Format_Y8
		   << QVideoFrame::Format_Y16
		   << QVideoFrame::Format_Jpeg
		   << QVideoFrame::Format_CameraRaw
		   << QVideoFrame::Format_AdobeDng;
}

bool PoorMansProbe::present(const QVideoFrame &frame)
{
	if (frame.isValid()) {
		emit videoFrameProbed(frame);
		return true;
	}
	return false;
}


/*
		QVideoFrame cloneFrame(frame);
		cloneFrame.map(QAbstractVideoBuffer::ReadOnly);
		const QImage image(cloneFrame.bits(),
						   cloneFrame.width(),
						   cloneFrame.height(),
						   QVideoFrame::imageFormatFromPixelFormat(cloneFrame .pixelFormat()));
		emit frameAvailable(image);
		cloneFrame.unmap();
		return true;
	}
*/


bool PoorMansProbe::setSource(QCamera *source)
{
	this->source=source;
	source->setViewfinder(this);
	QCameraViewfinderSettings viewfinderSettings;
	viewfinderSettings.setResolution(640, 480);
	viewfinderSettings.setMinimumFrameRate(0.0);
	viewfinderSettings.setMaximumFrameRate(30.0);
	source->setViewfinderSettings(viewfinderSettings);
	return true;
}


bool PoorMansProbe::isActive() const
{
	return (0!=source);
}
