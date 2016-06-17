#ifndef POORMANSPROBE_HPP
#define POORMANSPROBE_HPP


#include <QAbstractVideoSurface>
#include <QList>


class QCamera;
class QCameraViewfinder;


class PoorMansProbe : public QAbstractVideoSurface
{
		Q_OBJECT

	private:
		QCamera *source;
	public:
		explicit PoorMansProbe(QObject *parent = nullptr);

		QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const;

		bool present(const QVideoFrame &frame);

		bool setSource(QCamera *source);


		bool isActive() const;



	Q_SIGNALS:
		void videoFrameProbed(const QVideoFrame &videoFrame);
		void flush();



	public slots:

};



#endif // POORMANSPROBE_HPP
