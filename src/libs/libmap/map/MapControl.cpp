/*
*
* This file is part of QMapControl,
* an open-source cross-platform map widget
*
* Copyright (C) 2007 - 2008 Kai Winter
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with QMapControl. If not, see <http://www.gnu.org/licenses/>.
*
* Contact e-mail: kaiwinter@gmx.de
* Program URL   : http://qmapcontrol.sourceforge.net/
*
*/

#include "MapControl.hpp"
#include <QTimer>
#include <QBitmap>
namespace qmapcontrol
{
MapControl::MapControl (QWidget * parent, Qt::WindowFlags windowFlags)
	:   QWidget( parent, windowFlags ),
		mLayerManager(0),
		mImageManager(0),
		mSize(100,100),
		mMouseWheelEvents(true),
		mMousePressed(false),
		mMouseMode(Panning),
		mScaleVisible(false),
		mCrosshairsVisible(true),
		mLoadingFlag(false),
		mSteps(0),
		m_doubleBuffer(0)
{
	OC_METHODGATE();
	__init();
}

MapControl::MapControl(QSize size, MouseMode mousemode, bool showScale, bool showCrosshairs, QWidget * parent, Qt::WindowFlags windowFlags)
	:   QWidget( parent, windowFlags ),
		mLayerManager(0),
		mImageManager(0),
		mSize(size),
		mMouseWheelEvents(true),
		mMousePressed(false),
		mMouseMode(mousemode),
		mScaleVisible(showScale),
		mCrosshairsVisible(showCrosshairs),
		mLoadingFlag(false),
		mSteps(0),
		m_doubleBuffer(0)
{
	OC_METHODGATE();
	__init();
}

MapControl::~MapControl()
{
	OC_METHODGATE();
	if ( mLayerManager ) {
		mLayerManager->deleteLater();
		mLayerManager = 0;
	}

	if ( mImageManager ) {
		mImageManager->deleteLater();
		mImageManager = 0;
	}
}

void MapControl::__init()
{
	OC_METHODGATE();
	mLayerManager = OC_NEW LayerManager(this, mSize);
	mImageManager = OC_NEW ImageManager(this);
	mScreenMiddle = QPoint(mSize.width()/2, mSize.height()/2);

	mMousePressed = false;

	connect(mImageManager, SIGNAL(imageReceived()),
			this, SLOT(updateRequestNew()));

	connect(mImageManager, SIGNAL(loadingFinished()),
			this, SLOT(loadingFinished()));

	//this->setMaximumSize(size.width(), size.height());
	setMouseTracking(true);
}

void MapControl::enableMouseWheelEvents( bool enabled )
{
	OC_METHODGATE();
	mMouseWheelEvents = enabled;
}

bool MapControl::mouseWheelEventsEnabled()
{
	OC_METHODGATE();
	return mMouseWheelEvents;
}

QPointF MapControl::currentCoordinate() const
{
	OC_METHODGATE();
	return mLayerManager->currentCoordinate();
}

QSharedPointer<Layer> MapControl::layer(const QString& layername) const
{
	OC_METHODGATE();
	return mLayerManager->layer(layername);
}

QList<QString> MapControl::layers() const
{
	OC_METHODGATE();
	return mLayerManager->layers();
}

int MapControl::numberOfLayers() const
{
	OC_METHODGATE();
	return mLayerManager->layers().size();
}

void MapControl::followGeometry(const Geometry* geom) const
{
	OC_METHODGATE();
	if ( nullptr ==  geom ) {
		return;
	}

	//ensures only one signal is ever connected
	stopFollowing(geom);

	connect(geom, SIGNAL(positionChanged(Geometry*)),
			this, SLOT(positionChanged(Geometry*)));
}

void MapControl::positionChanged(Geometry* geom)
{
	OC_METHODGATE();
	if ( !mLayerManager->layer() || !mLayerManager->layer()->mapadapter() ) {
		qDebug() << "MapControl::positionChanged() - no layers configured";
		return;
	}

	Point* point = dynamic_cast<Point*>(geom);
	if (point!=0) {
		QPoint start = mLayerManager->layer()->mapadapter()->coordinateToDisplay(currentCoordinate());
		QPoint dest = mLayerManager->layer()->mapadapter()->coordinateToDisplay(point->coordinate());
		QPoint step = (dest-start);
		mLayerManager->scrollView(step);
		updateRequestNew();
	}
}

void MapControl::moveTo(QPointF coordinate)
{
	OC_METHODGATE();
	mTarget = coordinate;
	mSteps = 25;
	if (mMoveMutex.tryLock()) {
		QTimer::singleShot(40, this, SLOT(tick()));
	} else {
		// stopMove(coordinate);
		mMoveMutex.unlock();
	}
}
void MapControl::tick()
{
	OC_METHODGATE();
	if ( !mLayerManager->layer() || !mLayerManager->layer()->mapadapter() ) {
		qDebug() << "MapControl::tick() - no layers configured";
		return;
	}

	QPoint start = mLayerManager->layer()->mapadapter()->coordinateToDisplay(currentCoordinate());
	QPoint dest = mLayerManager->layer()->mapadapter()->coordinateToDisplay(mTarget);

	QPoint step = (dest-start)/mSteps;
	mLayerManager->scrollView(step);

	update();
	mLayerManager->updateRequest();
	mSteps--;
	if (mSteps>0) {
		QTimer::singleShot(50, this, SLOT(tick()));
	} else {
		mMoveMutex.unlock();
	}
}

void MapControl::paintEvent(QPaintEvent* evnt)
{
	OC_METHODGATE();
	Q_UNUSED(evnt);

	if ( nullptr == m_doubleBuffer ) {
		m_doubleBuffer =  OC_NEW QPixmap(width(), height());
	}
	//check for resize change
	else if ( m_doubleBuffer->width() != width() || m_doubleBuffer->height() != height() ) {
		delete m_doubleBuffer;
		m_doubleBuffer = OC_NEW QPixmap(width(), height());
	}

	QPainter dbPainter;
	dbPainter.begin(m_doubleBuffer);

	mLayerManager->drawImage(&dbPainter);
	mLayerManager->drawGeoms(&dbPainter);

	// draw scale
	if (mScaleVisible) {
		static QList<double> distanceList;
		if (distanceList.isEmpty()) {
			distanceList<<5000000<<2000000<<1000000<<1000000<<1000000<<100000<<100000<<50000<<50000<<10000<<10000<<10000<<1000<<1000<<500<<200<<100<<50<<25;
		}

		if (currentZoom() >= mLayerManager->minZoom() && distanceList.size() > currentZoom()) {
			double line;
			line = distanceList.at( currentZoom() ) / pow(2.0, 18-currentZoom() ) / 0.597164;

			// draw the scale
			dbPainter.setPen(Qt::black);
			QPoint p1(10,mSize.height()-20);
			QPoint p2((int)line,mSize.height()-20);
			dbPainter.drawLine(p1,p2);

			dbPainter.drawLine(10,mSize.height()-15, 10,mSize.height()-25);
			dbPainter.drawLine((int)line,mSize.height()-15, (int)line,mSize.height()-25);

			QString distance;
			if (distanceList.at(currentZoom()) >= 1000) {
				distance = QVariant( distanceList.at(currentZoom())/1000 )  .toString()+ " km";
			} else {
				distance = QVariant( distanceList.at(currentZoom()) ).toString() + " m";
			}

			dbPainter.drawText(QPoint((int)line+10,mSize.height()-15), distance);
		}
	}

	if (mCrosshairsVisible) {
		dbPainter.drawLine(mScreenMiddle.x(), mScreenMiddle.y()-10,
						   mScreenMiddle.x(), mScreenMiddle.y()+10); // |
		dbPainter.drawLine(mScreenMiddle.x()-10, mScreenMiddle.y(),
						   mScreenMiddle.x()+10, mScreenMiddle.y()); // -
	}

	if (mMousePressed && mMouseMode == Dragging) {
		QRect rect = QRect(mPreClickPixel, mCurrentMousePos);
		dbPainter.drawRect(rect);
	}
	dbPainter.end();
	QPainter painter;
	painter.begin( this );



	painter.drawPixmap( rect(), *m_doubleBuffer, m_doubleBuffer->rect() );

	painter.end();
}

// mouse events
void MapControl::mousePressEvent(QMouseEvent* evnt)
{
	OC_METHODGATE();
	mLayerManager->mouseEvent(evnt);

	if (mLayerManager->layers().size()>0) {
		if (evnt->button() == 1) {
			mMousePressed = true;
			mPreClickPixel = QPoint(evnt->x(), evnt->y());
		} else if ( evnt->button() == 2  &&
					mouseWheelEventsEnabled() &&
					mMouseMode != None) { // zoom in
			zoomIn();
		} else if  ( evnt->button() == 4 &&
					 mouseWheelEventsEnabled() &&
					 mMouseMode != None) { // zoom out
			zoomOut();
		}
	}

	// emit(mouseEvent(evnt));
	emit(mouseEventCoordinate(evnt, clickToWorldCoordinate(evnt->pos())));
}

void MapControl::mouseReleaseEvent(QMouseEvent* evnt)
{
	OC_METHODGATE();
	mMousePressed = false;
	if (mMouseMode == Dragging) {
		QPointF ulCoord = clickToWorldCoordinate(mPreClickPixel);
		QPointF lrCoord = clickToWorldCoordinate(mCurrentMousePos);

		QRectF coordinateBB = QRectF(ulCoord, QSizeF( (lrCoord-ulCoord).x(), (lrCoord-ulCoord).y()));

		emit(boxDragged(coordinateBB));
	}

	emit(mouseEventCoordinate(evnt, clickToWorldCoordinate(evnt->pos())));
}

void MapControl::mouseMoveEvent(QMouseEvent* evnt)
{
	OC_METHODGATE();
	if (mMousePressed && mMouseMode == Panning) {
		QPoint offset = mPreClickPixel - QPoint(evnt->x(), evnt->y());
		mLayerManager->scrollView(offset);
		mPreClickPixel = QPoint(evnt->x(), evnt->y());
	} else if (mMousePressed && mMouseMode == Dragging) {
		mCurrentMousePos = QPoint(evnt->x(), evnt->y());
	}

	update();
}

void MapControl::wheelEvent(QWheelEvent *evnt)
{
	OC_METHODGATE();
	if(mMouseWheelEvents && evnt->orientation() == Qt::Vertical) {
		QPointF pz=clickToWorldCoordinate(evnt->pos());
		QPointF pzo=currentCoordinate();
		QPointF pzd=pzo-pz;
		int d=evnt->delta();
		//	qDebug()<<"BEFORE: mouse="<<pz<<", map="<<pzo<<", delta="<<d;

		//QCursor::pos().
		if(d > 0) {
			if( currentZoom() == mLayerManager->maxZoom() ) {
				return;
			}
			zoomIn();
			pzd/=2.0;
			pzd+=pz;
			setView(pzd); //zoom in under mouse cursor
		} else  {
			if( currentZoom() == mLayerManager->minZoom() ) {
				return;
			}
			zoomOut();
			pzd*=2.0;
			pzd+=pz;
			setView(pzd); //zoom in under mouse cursor

		}
		evnt->accept();
		pzo=currentCoordinate();
		//	qDebug()<<"AFTER: mouse="<<pz<<", map="<<pzo<<", delta="<<d;
	} else {
		evnt->ignore();
	}

}


void MapControl::resizeEvent(QResizeEvent *e)
{
	OC_METHODGATE();
	delete m_doubleBuffer;
	m_doubleBuffer=nullptr;
	QSize sz=e->size();
	//sz-=QSize(1,1);
	resize(sz);
}

ImageManager* MapControl::getImageManager()
{
	return mImageManager;
}

QPointF MapControl::clickToWorldCoordinate(QPoint click)
{
	OC_METHODGATE();
	if ( !mLayerManager->layer() || !mLayerManager->layer()->mapadapter() ) {
		qDebug() << "MapControl::clickToWorldCoordinate() - no layers configured";
		return QPointF();
	}
	// click coordinate to image coordinate
	QPoint displayToImage= QPoint(click.x()-mScreenMiddle.x()+mLayerManager->getMapmiddle_px().x(),
								  click.y()-mScreenMiddle.y()+mLayerManager->getMapmiddle_px().y());

	// image coordinate to world coordinate
	return mLayerManager->layer()->mapadapter()->displayToCoordinate(displayToImage);
}

void MapControl::updateRequest(QRect rect)
{
	OC_METHODGATE();
	update(rect);
}

void MapControl::updateRequestNew()
{
	OC_METHODGATE();
	mLayerManager->forceRedraw();
}

// slots
void MapControl::zoomIn()
{
	OC_METHODGATE();
	mLayerManager->zoomIn();
	updateView();
	emit viewChanged(currentCoordinate(), currentZoom());
}

void MapControl::zoomOut()
{
	OC_METHODGATE();
	mLayerManager->zoomOut();
	updateView();
	emit viewChanged(currentCoordinate(), currentZoom());
}

void MapControl::setZoom(int zoomlevel)
{
	OC_METHODGATE();
	mLayerManager->setZoom(zoomlevel);
	updateView();
	emit viewChanged(currentCoordinate(), currentZoom());
}

int MapControl::currentZoom() const
{
	OC_METHODGATE();
	return mLayerManager->currentZoom();
}

void MapControl::scrollLeft(int pixel)
{
	OC_METHODGATE();
	mLayerManager->scrollView(QPoint(-pixel,0));
	updateView();
}

void MapControl::scrollRight(int pixel)
{
	OC_METHODGATE();
	mLayerManager->scrollView(QPoint(pixel,0));
	updateView();
}

void MapControl::scrollUp(int pixel)
{
	OC_METHODGATE();
	mLayerManager->scrollView(QPoint(0,-pixel));
	updateView();
}

void MapControl::scrollDown(int pixel)
{
	OC_METHODGATE();
	mLayerManager->scrollView(QPoint(0,pixel));
	updateView();
}

void MapControl::scroll(const QPoint scroll)
{
	OC_METHODGATE();
	mLayerManager->scrollView(scroll);
	updateView();
}

void MapControl::updateView() const
{
	OC_METHODGATE();
	mLayerManager->setView( currentCoordinate() );
	emit viewChanged(currentCoordinate(), currentZoom());
}

void MapControl::setView(const QPointF& coordinate) const
{
	OC_METHODGATE();
	mLayerManager->setView(coordinate);
	emit viewChanged(currentCoordinate(), currentZoom());
}

void MapControl::setView(const QList<QPointF> coordinates) const
{
	OC_METHODGATE();
	mLayerManager->setView(coordinates);
	emit viewChanged(currentCoordinate(), currentZoom());
}

void MapControl::setViewAndZoomIn(const QList<QPointF> coordinates) const
{
	OC_METHODGATE();
	mLayerManager->setViewAndZoomIn(coordinates);
	emit viewChanged(currentCoordinate(), currentZoom());
}

void MapControl::setView(const Point* point) const
{
	OC_METHODGATE();
	mLayerManager->setView(point->coordinate());
}

void MapControl::loadingFinished()
{
	OC_METHODGATE();
	mLayerManager->removeZoomImage();
}


void MapControl::clearLayers ()
{
	OC_METHODGATE();
	mLayerManager->clearLayers();
	update();
}

void MapControl::addLayer(QSharedPointer<Layer> layer)
{
	OC_METHODGATE();
	layer->setImageManager(mImageManager);
	mLayerManager->addLayer(layer);
	update();
}

void MapControl::removeLayer( QSharedPointer<Layer> layer )
{
	OC_METHODGATE();
	disconnect(layer.data(), 0, 0, 0);
	mLayerManager->removeLayer( layer );
	update();
}

void MapControl::setMouseMode(MouseMode mousemode)
{
	OC_METHODGATE();
	mMouseMode = mousemode;
}

MapControl::MouseMode MapControl::mouseMode()
{
	OC_METHODGATE();
	return mMouseMode;
}

void MapControl::stopFollowing(const Geometry* geom) const
{
	OC_METHODGATE();
	disconnect(geom,SIGNAL(positionChanged(Geometry*)), this, SLOT(positionChanged(Geometry*)));
}

void MapControl::enablePersistentCache( const QDir& path, const int qDiskSizeMiB )
{
	OC_METHODGATE();
	mImageManager->setCacheDir( path, qDiskSizeMiB );
}


void MapControl::setProxy(QString host, int port, const QString username, const QString password)
{
	OC_METHODGATE();
	mImageManager->setProxy(host, port, username, password);
}

void MapControl::showScale(bool visible)
{
	OC_METHODGATE();
	mScaleVisible = visible;
}

void MapControl::showCrosshairs(bool visible)
{
	OC_METHODGATE();
	mCrosshairsVisible = visible;
}

void MapControl::resize(const QSize newSize)
{
	OC_METHODGATE();
	this->mSize = newSize;
	mScreenMiddle = QPoint(newSize.width()/2, newSize.height()/2);
	//this->setMaximumSize(newSize.width(), newSize.height());
	mLayerManager->resize(newSize);
	emit viewChanged(currentCoordinate(), currentZoom());
}

void MapControl::setUseBoundingBox( bool usebounds )
{
	OC_METHODGATE();
	if( mLayerManager ) {
		mLayerManager->setUseBoundingBox( usebounds );
	}
}

bool MapControl::isBoundingBoxEnabled()
{
	OC_METHODGATE();
	if( mLayerManager ) {
		return mLayerManager->isBoundingBoxEnabled();
	}
	return false;
}

void MapControl::setBoundingBox( QRectF &rect )
{
	OC_METHODGATE();
	if( mLayerManager ) {
		mLayerManager->setBoundingBox( rect );
	}
}

QRectF MapControl::getBoundingBox()
{
	OC_METHODGATE();
	if( mLayerManager ) {
		return mLayerManager->getBoundingBox();
	}

	// Return an empty QRectF if there is no m_layermanager
	return QRectF();
}

QRectF MapControl::getViewport()
{
	OC_METHODGATE();
	if( mLayerManager ) {
		return mLayerManager->getViewport();
	}

	// Return an empty QRectF if there is no m_layermanager
	return QRectF();
}

bool MapControl::isGeometryVisible( Geometry * geometry)
{
	OC_METHODGATE();
	if ( !geometry || getViewport() == QRectF() ) {
		return false;
	}

	return getViewport().contains( geometry->boundingBox() );
}

int MapControl::loadingQueueSize()
{
	OC_METHODGATE();
	return mImageManager->loadQueueSize();
}

}
