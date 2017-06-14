#include "ImageLabel2.hpp"


#include <QPainter>

ImageLabel2::ImageLabel2(QWidget *parent)
	: QLabel(parent)
{
	this->setMinimumSize(1,1);
}


void ImageLabel2::paintEvent(QPaintEvent *aEvent){
	QLabel::paintEvent(aEvent);
	render();
}


void ImageLabel2::setPixmap(const QPixmap &aPicture)
{
	//Don't tell our ancestors that we got a pixmap!
	_qpSource = _qpCurrent = aPicture;
	repaint();
}



void ImageLabel2::render()
{

	if ( _qpSource.isNull()) //no image was set, don't draw anything
		return;

	float cw = width(), ch = height();
	float pw = _qpCurrent.width(), ph = _qpCurrent.height();

	if ( ( pw > cw && ph > ch && pw/cw > ph/ch ) || //both width and high are bigger, ratio at high is bigger or
		 (pw > cw && ph <= ch) || //only the width is bigger or
		 (pw < cw && ph < ch && cw/pw < ch/ph) //both width and height is smaller, ratio at width is smaller
		 ){
		_qpCurrent = _qpSource.scaledToWidth(cw, Qt::TransformationMode::SmoothTransformation);
	}
	else if ( (pw > cw && ph > ch && pw/cw <= ph/ch) || //both width and high are bigger, ratio at width is bigger or
			  (ph > ch && pw <= cw) || //only the height is bigger or
			  (pw < cw && ph < ch && cw/pw > ch/ph) //both width and height is smaller, ratio at height is smaller
			  ){
		_qpCurrent = _qpSource.scaledToHeight(ch, Qt::TransformationMode::SmoothTransformation);
	}
	else{
		//_qpCurrent = _qpSource;
	}

	int x = (cw - _qpCurrent.width())/2;
	int y = (ch - _qpCurrent.height())/2;
	QPainter paint(this);
	paint.drawPixmap(x, y, _qpCurrent);
}
