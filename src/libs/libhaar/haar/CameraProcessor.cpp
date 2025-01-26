#include "CameraProcessor.hpp"

#include "FormatUtils.hpp"
#include "HaarCascadeModel.hpp"
#include "HaarFeatureExtractor.hpp"
#include "HaarLoaderXML.hpp"
#include "HaarPreprocessor.hpp"
#include "HaarRendering.hpp"
#include "uptime/New.hpp"


#include <QCameraDevice>
#include <QComboBox>
#include <QFileInfo>
#include <QImage>
#include <QLoggingCategory>
#include <QMediaDevices>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QAbstractItemView>


const int CameraProcessor::PREVIEW_SIZE{128};
const int CameraProcessor::DEFAULT_SIZE{24};


class CustomIconDelegate : public QStyledItemDelegate {
public:
	CustomIconDelegate(const QSize &iconSize, QObject *parent = nullptr)
		: QStyledItemDelegate(parent), m_iconSize(iconSize) {}
	
	void initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const override {
		QStyledItemDelegate::initStyleOption(option, index);
		option->decorationSize = m_iconSize; // Set the desired icon size for the popup
	}
	
private:
	QSize m_iconSize;
};



CameraProcessor::CameraProcessor(QWidget *parent)
	: QWidget(parent)
	, m_camera(nullptr)
	, m_videoSink(new QVideoSink(this))
	, m_previewLabel(new QLabel(this))
	, m_cameraSelector(new QComboBox(this))
	, m_cascadeSelector(new QComboBox(this)) // Still using QComboBox
	, m_cascadeModel(new HaarCascadeModel(this)) // Model to manage Haar cascades
	, mExtractor(OC_NEW HaarFeatureExtractor())
{
	auto layout = new QVBoxLayout(this);
	
	// Camera Selector
	layout->addWidget(m_cameraSelector);
	m_cameraSelector->addItem("No Camera");
	const QList<QCameraDevice> devices = QMediaDevices::videoInputs();
	for (const QCameraDevice &device : devices) {
		m_cameraSelector->addItem(device.description());
	}
	connect(m_cameraSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CameraProcessor::switchCamera);
	
	// Haar Cascade Selector
	layout->addWidget(m_cascadeSelector);
	m_cascadeSelector->setModel(m_cascadeModel); // Set the model for the combo box
	connect(m_cascadeSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CameraProcessor::cascadeChanged);
	
	// Add Haar cascades
	const QString base = QString("/home/leo/Desktop/");
	addCascade(base + "haarcascade_frontalface_default.xml");
	addCascade(base + "haarcascade_frontalface_alt.xml");
	
	// Preview Label
	m_previewLabel->setAlignment(Qt::AlignCenter);
	m_previewLabel->setScaledContents(false);
	layout->addWidget(m_previewLabel);
	
	// Connect video frame to handleFrame
	connect(m_videoSink, &QVideoSink::videoFrameChanged, this, &CameraProcessor::handleFrame);
	
	//m_cascadeSelector->setIconSize(QSize(PREVIEW_SIZE, PREVIEW_SIZE));
	
	m_cascadeSelector->setIconSize(QSize(DEFAULT_SIZE, DEFAULT_SIZE)); // Default size for closed combo box
	m_cascadeSelector->view()->setItemDelegate(new CustomIconDelegate(QSize(PREVIEW_SIZE, PREVIEW_SIZE), m_cascadeSelector)); // Larger size for popup
	
}



void CameraProcessor::addCascade(const QString &filePath) {
	auto cascade = loader.load(filePath);
	if (cascade) {
		m_cascadeModel->addCascade(cascade);
	}
	else{
		qWarning()<<"Could not add cascade from file" << filePath;
	}
}

void CameraProcessor::cascadeChanged(int index) {
	if (index < 0) {
		qDebug() << "Invalid cascade index";
		mCurrentCascade.reset();
		return;
	}
	
	auto item = m_cascadeModel->cascadeAt(index);
	if (item) {
		mCurrentCascade = item;
		qDebug() << "Selected Haar Cascade:" << mCurrentCascade->name;
	} else {
		mCurrentCascade.reset();
		qDebug() << "No valid Haar Cascade selected.";
	}
}


CameraProcessor::~CameraProcessor()
{
	if (m_camera) {
		m_camera->stop();
		delete m_camera;
	}
}

void CameraProcessor::switchCamera(int index)
{
	const QList<QCameraDevice> devices = QMediaDevices::videoInputs();
	if (index == 0) {
		if (m_camera) {
			m_camera->stop();
			delete m_camera;
			m_camera = nullptr;
		}
		m_previewLabel->clear();
		return;
	}
	int deviceIndex = index - 1;
	if (deviceIndex >= 0 && deviceIndex < devices.size()) {
		setupCamera(devices[deviceIndex]);
	}
}

void CameraProcessor::setupCamera(const QCameraDevice &cameraDevice)
{
	if (m_camera) {
		m_camera->stop();
		delete m_camera;
	}
	qDebug()<<"Starting camera"<< cameraDevice;
	m_camera = new QCamera(cameraDevice, this);
	m_captureSession.setCamera(m_camera);
	m_captureSession.setVideoSink(m_videoSink);
	QCameraFormat cameraFormat = cameraDevice.videoFormats().first();
	QSize nativeResolution = cameraFormat.resolution();
	if (!nativeResolution.isEmpty()) {
		int scaleFactor = 1;
		m_previewLabel->setMinimumSize(nativeResolution.width() * scaleFactor, nativeResolution.height() * scaleFactor);
	}
	m_camera->start();
}

static inline QImage generateErrorImage(const QSize &size, const QString &text) {
	QImage image(size, QImage::Format_RGBA8888);
	image.fill(Qt::darkGray);
	QPainter painter(&image);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setRenderHint(QPainter::TextAntialiasing);
	painter.setPen(Qt::red);
	QFont font = painter.font();
	font.setPointSize(20);
	font.setBold(true);
	painter.setFont(font);
	QRect textRect = QRect(QPoint(0, 0), size);
	painter.drawText(textRect, Qt::AlignCenter, text);
	return image;
}


static inline QImage videoFrameToImage(QVideoFrame &frame)
{
	if (!frame.map(QVideoFrame::ReadOnly)) {
		qDebug() << "Could not map image";
		return generateErrorImage(frame.size(), "Could not map image");
	}
	auto pixelFormat = frame.pixelFormat();
	auto imageFormat = QVideoFrameFormat::imageFormatFromPixelFormat(pixelFormat);
	if (imageFormat == QImage::Format_Invalid) {
		if (pixelFormat == QVideoFrameFormat::Format_Jpeg) {
			auto jpegImage = QImage::fromData(frame.bits(0), frame.mappedBytes(0));
			if (!jpegImage.isNull()) {
				frame.unmap();
				return jpegImage;
			} else {
				qDebug() << "JPEG decoding error";
				frame.unmap();
				return generateErrorImage(frame.size(), "JPEG decoding error");
			}
		} else {
			qDebug() << "Image format invalid";
			frame.unmap();
			return generateErrorImage(frame.size(), QString("Format conversion error: %1 -> %2").arg(videoFormatToString(pixelFormat)).arg(imageFormatToString(imageFormat)) );
		}
	}
	QImage image(frame.bits(0), frame.width(), frame.height(), frame.bytesPerLine(0), imageFormat);
	frame.unmap();
	return image;
}


void CameraProcessor::handleFrame(const QVideoFrame &frame)
{
	if (!frame.isValid()){
		qDebug()<<"Invalid frame";
		return;
	}
	QVideoFrame mutableFrame = frame;
	QImage image = videoFrameToImage(mutableFrame);
	if (!image.isNull()) {
		
		auto preprocessed = preprocessImage(image);
		{
			QPainter painter(&image);
			painter.drawImage(0, 0, preprocessed);
		}
		
		mExtractor->ingest(image);
		auto features = mExtractor->extract(mCurrentCascade);
		{
			QPainter painter(&image);
			painter.translate(200, 200);
			drawHaarCascade(painter, *mCurrentCascade.data());
			painter.setPen(Qt::red);
			painter.fillRect(0, 0, 100, 100, Qt::green);
			painter.drawText(10, 10, "BALLS");
			for(auto feature:features){
				painter.drawRect(feature);
			}
		}
			
		m_previewLabel->setPixmap(QPixmap::fromImage(image));
		//qDebug()<<"Got image: " << image.size();
	}
	else{
		qDebug()<<"Null image";
	}
}


