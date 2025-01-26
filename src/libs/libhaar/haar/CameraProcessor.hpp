#ifndef CAMERAPROCESSOR_HPP
#define CAMERAPROCESSOR_HPP

#include <QCamera>
#include <QComboBox>
#include <QImage>
#include <QLabel>
#include <QMediaCaptureSession>
#include <QPushButton>
#include <QVBoxLayout>
#include <QVideoFrame>
#include <QVideoSink>
#include <QWidget>

#include "HaarCascade.hpp"
#include "haar/HaarLoaderXML.hpp"



class HaarFeatureExtractor;
class HaarCascadeModel;

class CameraProcessor : public QWidget
{
	Q_OBJECT
public:
	static const int PREVIEW_SIZE;
	static const int DEFAULT_SIZE;
private:
	QCamera *m_camera{nullptr};
	QMediaCaptureSession m_captureSession;
	QVideoSink *m_videoSink{nullptr};
	QLabel *m_previewLabel{nullptr};
	QComboBox *m_cameraSelector{nullptr};
	QComboBox *m_cascadeSelector{nullptr};
	HaarCascadeModel *m_cascadeModel{nullptr};
	HaarFeatureExtractor *mExtractor{nullptr};
	HaarLoaderXML loader;
	QSharedPointer<HaarCascade> mCurrentCascade;
	
public:
	explicit CameraProcessor(QWidget *parent = nullptr);
	~CameraProcessor();
	
private slots:
	void handleFrame(const QVideoFrame &frame);
	void switchCamera(int index);
	void cascadeChanged(int index);
	
private:
	void setupCamera(const QCameraDevice &cameraDevice);
	void addCascade(const QString &filePath);
	//void addCascade(const QString &name, QSharedPointer<HaarCascade> cascade);
};

#endif // CAMERAPROCESSOR_HPP
