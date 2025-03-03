#include "PulsatingLabel.hpp"
#include <QPainter>
#include <QSvgRenderer>
#include <QDebug>

PulsatingLabel::PulsatingLabel(QWidget *parent)
	: QLabel(parent)
	, m_scaleFactor(1.0)
	, m_svgRenderer(nullptr)
{
}

qreal PulsatingLabel::scaleFactor() const {
	return m_scaleFactor;
}

void PulsatingLabel::setScaleFactor(qreal factor) {
	if (qFuzzyCompare(m_scaleFactor, factor))
		return;
	m_scaleFactor = factor;
	emit scaleFactorChanged(m_scaleFactor);
	update();
}

void PulsatingLabel::setSvg(const QString &svgPath) {
	// Clean up previous renderer if any
	if (m_svgRenderer) {
		delete m_svgRenderer;
		m_svgRenderer = nullptr;
	}
	m_svgRenderer = new QSvgRenderer(svgPath, this);
	if (!m_svgRenderer->isValid()) {
		qWarning() << "Failed to load SVG:" << svgPath;
		delete m_svgRenderer;
		m_svgRenderer = nullptr;
	} else {
		update();
	}
}


void PulsatingLabel::paintEvent(QPaintEvent *event) {
	Q_UNUSED(event);
	QPainter painter(this);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
	
	// Use the widget's available size as the base.
	QSize availableSize = size();
	QSize desiredSize;
	
	// Determine the intrinsic size from the SVG or pixmap,
	// then scale it to fit within availableSize while maintaining aspect ratio.
	if (m_svgRenderer && m_svgRenderer->isValid()) {
		QSize origSize = m_svgRenderer->defaultSize();
		desiredSize = origSize.scaled(availableSize, Qt::KeepAspectRatio);
	} else if (!pixmap().isNull()) {
		desiredSize = pixmap().size().scaled(availableSize, Qt::KeepAspectRatio);
	} else {
		desiredSize = availableSize;
	}
	
	// Apply the pulsation scale factor.
	QSize finalSize(static_cast<int>(desiredSize.width() * m_scaleFactor),
					static_cast<int>(desiredSize.height() * m_scaleFactor));
	
	// Center the final size in the widget.
	QPoint center = rect().center();
	QRect drawRect(center.x() - finalSize.width() / 2,
				   center.y() - finalSize.height() / 2,
				   finalSize.width(), finalSize.height());
	
	// Render the image.
	if (m_svgRenderer && m_svgRenderer->isValid()) {
		m_svgRenderer->render(&painter, drawRect);
	} else if (!pixmap().isNull()) {
		painter.drawPixmap(drawRect, pixmap());
	}
}
