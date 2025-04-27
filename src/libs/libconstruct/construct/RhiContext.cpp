#include "RhiContext.hpp"

#include <rhi/qrhi.h>

#include <QTextStream>
#include <QSize>


RhiContext::RhiContext(){
	//qDebug() << "RhiContext::ctor()";
}


void RhiContext::updateViewMatrix(){
	//qDebug() << "RhiContext::updateViewMatrix()";
	
	// Compute the projection matrix (with clip-space correction)
	QMatrix4x4 proj = m_rhi->clipSpaceCorrMatrix();
	proj.perspective(45.0f, m_pixelSize.width() / float(m_pixelSize.height()), 0.1f, 1000.0f);
	
	// Compute the view matrix
	QMatrix4x4 view;
	view.setToIdentity();
	view.translate(0, 0, -5);
	
	// Combine them in the correct order: projection * view
	m_viewProjMatrix = proj * view;
}


QRhiResourceUpdateBatch *RhiContext::ensureResourceUpdates(){
	if(nullptr==resourceUpdates){
		resourceUpdates = m_rhi->nextResourceUpdateBatch();
	}
	return resourceUpdates;
}


QTextStream &operator<<(QTextStream &stream, const QSize &size)
{
	stream << "QSize(" << size.width() << ", " << size.height() << ")";
	return stream;
}


QString RhiContext::toString() const
{
	QString result;
	QTextStream stream(&result);
	
	stream << "RhiContext("
		   << "sampleCount: " << m_sampleCount << ", "
		   << "pixelSize: " << m_pixelSize
		   << ")";
	
	return result;
}


QTextStream &operator<<(QTextStream &stream, const RhiContext &context)
{
	stream << context.toString();
	return stream;
}


QDebug operator<<(QDebug dbg, const RhiContext &context)
{
	QDebugStateSaver saver(dbg);
	dbg.nospace() << context.toString();
	return dbg;
}




