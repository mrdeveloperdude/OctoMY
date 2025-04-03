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

#include "LayerManager.hpp"


namespace qmapcontrol
{
LayerManager::LayerManager(MapControl* mapcontrol, QSize size)
	:mMapControl(mapcontrol), mScroll(QPoint(0,0)), mSize(size), mWhileNewScroll(QPoint(0,0))
{
	// genauer berechnen?
	mOffScreenSize = size *2;
	mComposedOffscreenImage = QPixmap(mOffScreenSize);
	mZoomImage = QPixmap(size);
	mZoomImage.fill(Qt::white);
	mScreenMiddle = QPoint(size.width()/2, size.height()/2);
	mUseBoundingBox = false;
}


LayerManager::~LayerManager()
{
	mylayers.clear();
}

QPointF LayerManager::currentCoordinate() const
{
	return mMapMiddle;
}

QPixmap LayerManager::getImage() const
{
	return mComposedOffscreenImage;
}

QSharedPointer<Layer> LayerManager::layer() const
{
	if ( mylayers.isEmpty() ) {
		qDebug() << "LayerManager::getLayer() - No layers were added";
		return 0;
	}
	return mylayers.at(0) ? mylayers.at(0) : 0;
}

QSharedPointer<Layer> LayerManager::layer(const QString& layername) const
{
	QListIterator<QSharedPointer<Layer> > layerit(mylayers);
	while (layerit.hasNext()) {
		QSharedPointer<Layer> l = layerit.next();
		if (l->layername() == layername) {
			return l;
		}
	}
	return 0;
}

QList<QString> LayerManager::layers() const
{
	QList<QString> keys;
	QListIterator<QSharedPointer<Layer> > layerit(mylayers);
	while (layerit.hasNext()) {
		keys.append(layerit.next()->layername());
	}
	return keys;
}


void LayerManager::scrollView(const QPoint& point)
{
	QPointF tempMiddle = layer()->mapadapter()->displayToCoordinate(mMapMiddlePixels + point);

	if((mUseBoundingBox && mBoundingBox.contains(tempMiddle)) || !mUseBoundingBox) {
		mScroll += point;
		mZoomImageScroll += point;
		mMapMiddlePixels += point;

		mMapMiddle = tempMiddle;

		if (!checkOffscreen()) {
			newOffscreenImage();
		} else {
			moveWidgets();
		}
	}
}

void LayerManager::moveWidgets()
{
	QListIterator<QSharedPointer<Layer> > it(mylayers);
	while (it.hasNext()) {
		it.next()->moveWidgets(mMapMiddlePixels);
	}
}

void LayerManager::setView(const QPointF& coordinate)
{
	if ( !layer() ) {
		qDebug() << "LayerManager::setView() - cannot set view settings with no layers configured";
		return;
	}

	if ( !layer()->mapadapter() ) {
		qDebug() << "LayerManager::setView() - cannot set view settings with no map adapter configured";
		return;
	}

	mMapMiddlePixels = layer()->mapadapter()->coordinateToDisplay(coordinate);
	mMapMiddle = coordinate;

	newOffscreenImage(true, false);
}

void LayerManager::setView(QList<QPointF> coordinates)
{
	setMiddle(coordinates);
	mMapControl->update();
}

void LayerManager::setViewAndZoomIn(const QList<QPointF> coordinates)
{
	if ( !layer() ) {
		qDebug() << "LayerManager::setViewAndZoomIn() - no layers configured";
		return;
	}


	while (!containsAll(coordinates)) {
		setMiddle(coordinates);
		zoomOut();
		// bugfix Tl
		// if points are too close -> Loop of death...
		if ( layer()->mapadapter()->currentZoom() == 0 ) {
			//qDebug() << "LayerManager::setViewAndZoomIn() - reached minium zoom level";
			break;
		}
	}

	while (containsAll(coordinates)) {
		setMiddle(coordinates);
		zoomIn();
		// bugfix Tl
		// if points are too close -> Loop of death...
		if ( layer()->mapadapter()->currentZoom() == 17 ) {
			//qDebug() << "LayerManager::setViewAndZoomIn() - reached maximum zoom level";
			break;
		}
	}

	if (!containsAll(coordinates)) {
		zoomOut();
	}

	mMapControl->update();
}

void LayerManager::setMiddle(QList<QPointF> coordinates)
{
	if ( !layer() ) {
		qDebug() << "LayerManager::setMiddle() - no layers configured";
		return;
	}

	int sum_x = 0;
	int sum_y = 0;
	for (int i=0; i<coordinates.size(); ++i) {
		// mitte muss in px umgerechnet werden, da aufgrund der projektion die mittebestimmung aus koordinaten ungenau ist
		QPoint p = layer()->mapadapter()->coordinateToDisplay(coordinates.at(i));
		sum_x += p.x();
		sum_y += p.y();
	}
	QPointF middle = layer()->mapadapter()->displayToCoordinate(QPoint(sum_x/coordinates.size(), sum_y/coordinates.size()));
	// middle in px rechnen!

	setView(middle);
}

bool LayerManager::containsAll(QList<QPointF> coordinates) const
{
	QRectF bb = getViewport();
	bool containsall = true;
	for (int i=0; i<coordinates.size(); ++i) {
		if (!bb.contains(coordinates.at(i))) {
			return false;
		}
	}
	return containsall;
}

QPoint LayerManager::getMapmiddle_px() const
{
	return mMapMiddlePixels;
}

QRectF LayerManager::getViewport() const
{
	if ( !layer() ) {
		//qDebug() << "LayerManager::getViewport() - no layers configured";
		return QRectF();
	}

	if ( !layer()->mapadapter() ) {
		//qDebug() << "LayerManager::getViewport() - no mapadapter";
		return QRectF();
	}

	QPoint upperLeft = QPoint(mMapMiddlePixels.x()-mScreenMiddle.x(), mMapMiddlePixels.y()+mScreenMiddle.y());
	QPoint lowerRight = QPoint(mMapMiddlePixels.x()+mScreenMiddle.x(), mMapMiddlePixels.y()-mScreenMiddle.y());

	QPointF ulCoord = layer()->mapadapter()->displayToCoordinate(upperLeft);
	QPointF lrCoord = layer()->mapadapter()->displayToCoordinate(lowerRight);

	QRectF coordinateBB = QRectF(ulCoord, QSizeF( (lrCoord-ulCoord).x(), (lrCoord-ulCoord).y()));
	return coordinateBB;
}

void LayerManager::addLayer(QSharedPointer<Layer> layer)
{
	mylayers.append(layer);

	layer->setSize(mSize);

	//sanity check first
	disconnect( layer.data(), 0, this, 0 );

	connect(layer.data(), SIGNAL(updateRequest(QRectF)),
			this, SLOT(updateRequest(QRectF)));
	connect(layer.data(), SIGNAL(updateRequest()),
			this, SLOT(updateRequest()));

	if (mylayers.size() > 0) {
		//setView(QPointF(0,0));
	}
	mMapControl->update();
}


void LayerManager::clearLayers ()
{
	for(auto &layer: mylayers) {
		removeLayer(layer);
	}
}

void LayerManager::removeLayer(QSharedPointer<Layer> layer)
{
	if ( layer ) {
		disconnect( layer.data(), 0, this, 0 );
		mylayers.removeAll(layer);
	}

	if (mylayers.size() > 0) {
		//setView(QPointF(0,0));
	}
	mMapControl->update();
}

void LayerManager::newOffscreenImage(bool clearImage, bool showZoomImage)
{
	// 	qDebug() << "LayerManager::newOffscreenImage()";
	mWhileNewScroll = mMapMiddlePixels;

	if (clearImage || mMapControl->getImageManager()->loadQueueSize() == 0) {
		mComposedOffscreenImage.fill(Qt::white);
	}

	QPainter painter(&mComposedOffscreenImage);
	if (showZoomImage|| mMapControl->getImageManager()->loadQueueSize() != 0) {
		painter.drawPixmap(mScreenMiddle.x()-mZoomImageScroll.x(), mScreenMiddle.y()-mZoomImageScroll.y(),mZoomImage);
	}

	//only draw basemaps
	foreach(const QSharedPointer<Layer> l, mylayers) {
		if (l->isVisible() && l->layertype() == Layer::MapLayer) {
			l->drawYourImage(&painter, mWhileNewScroll);
		}
	}

	//stop the painter now that we've finished drawing
	painter.end();

	//composedOffscreenImage = composedOffscreenImage2;
	mScroll = mMapMiddlePixels-mWhileNewScroll;

	mMapControl->update();
}

void LayerManager::zoomIn()
{
	if ( !layer() ) {
		qDebug() << "LayerManager::zoomIn() - no layers configured";
		return;
	}

	mMapControl->getImageManager()->abortLoading();
	//QCoreApplication::processEvents();

	// layer rendern abbrechen?
	mZoomImageScroll = QPoint(0,0);

	mZoomImage.fill(Qt::white);
	QPixmap tmpImg = mComposedOffscreenImage.copy(mScreenMiddle.x()+mScroll.x(),mScreenMiddle.y()+mScroll.y(), mSize.width(), mSize.height());

	QPainter painter(&mZoomImage);
	painter.translate(mScreenMiddle);
	painter.scale(2, 2);
	painter.translate(-mScreenMiddle);

	painter.drawPixmap(0,0,tmpImg);

	QListIterator<QSharedPointer<Layer> > it(mylayers);
	//TODO: remove hack, that mapadapters wont get set zoom multiple times
	QList< QSharedPointer<MapAdapter> > doneadapters;
	while (it.hasNext()) {
		QSharedPointer<Layer> l = it.next();
		if (!doneadapters.contains(l->mapadapter())) {
			l->zoomIn();
			doneadapters.append(l->mapadapter());
		}
	}
	mMapMiddlePixels = layer()->mapadapter()->coordinateToDisplay(mMapMiddle);
	mWhileNewScroll = mMapMiddlePixels;

	newOffscreenImage(true, true); //show zoomed image while map loads
}

bool LayerManager::checkOffscreen() const
{
	if ( !layer() ) {
		qDebug() << "LayerManager::checkOffscreen() - no layers configured";
		return true;
	}

	// calculate offscreenImage dimension (px)
	QPoint upperLeft = mMapMiddlePixels - mScreenMiddle;
	QPoint lowerRight = mMapMiddlePixels + mScreenMiddle;
	QRect viewport = QRect(upperLeft, lowerRight);

	QRect testRect = layer()->offscreenViewport();

	if (!testRect.contains(viewport)) {
		return false;
	}

	return true;
}
void LayerManager::zoomOut()
{
	if ( !layer() ) {
		qDebug() << "LayerManager::zoomOut() - no layers configured";
		return;
	}

	//QCoreApplication::processEvents();
	mMapControl->getImageManager()->abortLoading();
	mZoomImageScroll = QPoint(0,0);
	mZoomImage.fill(Qt::white);
	QPixmap tmpImg = mComposedOffscreenImage.copy(mScreenMiddle.x()+mScroll.x(),mScreenMiddle.y()+mScroll.y(), mSize.width(), mSize.height());
	QPainter painter(&mZoomImage);
	painter.translate(mScreenMiddle);
	painter.scale(0.500001,0.500001);
	painter.translate(-mScreenMiddle);
	painter.drawPixmap(0,0,tmpImg);

	painter.translate(mScreenMiddle);
	painter.scale(2,2);
	painter.translate(-mScreenMiddle);

	QListIterator<QSharedPointer<Layer> > it(mylayers);
	//TODO: remove hack, that mapadapters wont get set zoom multiple times
	QList< QSharedPointer<MapAdapter> > doneadapters;
	while (it.hasNext()) {
		QSharedPointer<Layer> l = it.next();
		if (!doneadapters.contains(l->mapadapter())) {
			l->zoomOut();
			doneadapters.append(l->mapadapter());
		}
	}
	mMapMiddlePixels = layer()->mapadapter()->coordinateToDisplay(mMapMiddle);
	mWhileNewScroll = mMapMiddlePixels;

	newOffscreenImage(true, true); //show zoomed image while map loads
}

void LayerManager::setZoom(int zoomlevel)
{
	if ( !layer() ) {
		qDebug() << "LayerManager::setZoom() - no layers configured";
		return;
	}

	int current_zoom;
	if (layer()->mapadapter()->minZoom() < layer()->mapadapter()->maxZoom()) {
		current_zoom = layer()->mapadapter()->currentZoom();
	} else {
		current_zoom = layer()->mapadapter()->minZoom() - layer()->mapadapter()->currentZoom();
	}


	if (zoomlevel < current_zoom) {
		for (int i=current_zoom; i>zoomlevel; --i) {
			zoomOut();
		}
	} else {
		for (int i=current_zoom; i<zoomlevel; ++i) {
			zoomIn();
		}
	}
}

void LayerManager::mouseEvent(const QMouseEvent* evnt)
{
	if ( mMapControl && !mMapControl->mouseWheelEventsEnabled() ) {
		return;
	}

	foreach( QSharedPointer<Layer> l, mylayers ) {
		if (l && l->isVisible() ) {
			l->mouseEvent(evnt, mMapMiddlePixels);
		}
	}
}

void LayerManager::updateRequest(QRectF rect)
{
	if (getViewport().contains(rect.topLeft()) || getViewport().contains(rect.bottomRight())) {
		// QPoint point = getLayer()->getMapAdapter()->coordinateToDisplay(c);
		// const QPoint topleft = mapmiddle_px - screenmiddle;
		// QPoint finalpoint = point-topleft;
		// QRect rect_px = QRect(int(finalpoint.x()-(rect.width()-1)/2), int(finalpoint.y()-(rect.height()-1)/2),
		//  int(rect.width()+1), int(rect.height()+1));
		//
		// mapcontrol->updateRequest(rect_px);
		newOffscreenImage(false, false);
	}
}
void LayerManager::updateRequest()
{
	newOffscreenImage(true, false);
}
void LayerManager::forceRedraw()
{
	newOffscreenImage(true, false);
}
void LayerManager::removeZoomImage()
{
	mZoomImage.fill(Qt::white);
	forceRedraw();
}

void LayerManager::drawGeoms(QPainter* painter)
{
	if ( !layer() ) {
		qDebug() << "LayerManager::drawGeoms() - no layers configured";
		return;
	}
	QListIterator<QSharedPointer<Layer> > it(mylayers);
	while (it.hasNext()) {
		QSharedPointer<Layer> l = it.next();
		if (l->layertype() == Layer::GeometryLayer && l->isVisible()) {
			l->drawYourGeometries(painter, mMapMiddlePixels, layer()->offscreenViewport());
		}
	}
}
void LayerManager::drawImage(QPainter* painter)
{
	painter->drawPixmap(-mScroll.x()-mScreenMiddle.x(),
						-mScroll.y()-mScreenMiddle.y(),
						mComposedOffscreenImage);
}

int LayerManager::currentZoom() const
{
	if ( !layer() ) {
		qDebug() << "LayerManager::currentZoom() - no layers configured";
		return 0;
	}
	return layer()->mapadapter()->currentZoom();
}

int LayerManager::minZoom()
{
	if ( !layer() ) {
		qDebug() << "LayerManager::minZoom() - no layers configured";
		return 0;
	}
	return layer()->mapadapter()->minZoom();
}

int LayerManager::maxZoom()
{
	if ( !layer() ) {
		qDebug() << "LayerManager::maxZoom() - no layers configured";
		return 0;
	}
	return layer()->mapadapter()->maxZoom();
}

void LayerManager::resize(QSize newSize)
{
	mSize = newSize;
	mOffScreenSize = newSize *2;
	mComposedOffscreenImage = QPixmap(mOffScreenSize);
	mZoomImage = QPixmap(newSize);

	mScreenMiddle = QPoint(newSize.width()/2, newSize.height()/2);

	QListIterator<QSharedPointer<Layer> > it(mylayers);
	while (it.hasNext()) {
		QSharedPointer<Layer> l = it.next();
		l->setSize(newSize);
	}

	forceRedraw();
}

void LayerManager::setUseBoundingBox( bool usebounds )
{
	mUseBoundingBox = usebounds;
}

bool LayerManager::isBoundingBoxEnabled()
{
	return mUseBoundingBox;
}

void LayerManager::setBoundingBox( QRectF &rect )
{
	if( rect.right() < rect.left() ) {
		qDebug() << "LayerManager::setBoundingBox() - min longitude is bigger than max";
	}

	if( rect.top() < rect.bottom() ) {
		qDebug() << "LayerManager::setBoundingBox() - min latitude is bigger than max";
	}

	mBoundingBox = rect;
}

QRectF LayerManager::getBoundingBox()
{
	return mBoundingBox;
}
}
