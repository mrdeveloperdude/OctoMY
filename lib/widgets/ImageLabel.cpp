#include "ImageLabel.hpp"


ImageLabel::ImageLabel(QWidget *parent) :
	QLabel(parent)
{
	this->setMinimumSize(1,1);
}

void ImageLabel::setPixmap ( const QPixmap & p)
{
	pix = p;
	QLabel::setPixmap(p);
}

int ImageLabel::heightForWidth( int width ) const
{
	return ((qreal)pix.height()*width)/pix.width();
}

QSize ImageLabel::sizeHint() const
{
	int w = this->width();
	return QSize( w, heightForWidth(w) );
}

void ImageLabel::resizeEvent(QResizeEvent * )
{
	QLabel::setPixmap(pix.scaled(this->size(),  Qt::KeepAspectRatio, Qt::SmoothTransformation));
}


