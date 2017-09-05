#include "MapEditor.hpp"
#include "ui_MapEditor.h"

#include "map/MapControl.hpp"
#include "map/OSMMapAdapter.hpp"
#include "map/OpenAerialMapAdapter.hpp"
#include "map/MapLayer.hpp"
#include "map/GoogleMapAdapter.hpp"
#include "map/LineString.hpp"



#include <QGeoPositionInfoSource>

MapEditor::MapEditor(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::MapEditor)
	, gps(QGeoPositionInfoSource::createDefaultSource(this))
{
	ui->setupUi(this);
	if (0 != gps) {
		gps->setPreferredPositioningMethods(QGeoPositionInfoSource::SatellitePositioningMethods);
		if(0!=gps){
			if(!connect(gps, SIGNAL(positionUpdated(QGeoPositionInfo)),this,SLOT(onPositionUpdated(QGeoPositionInfo)))){
				qWarning()<<"ERROR: Could not connect";
			}
		}
	}
	else{
		ui->toolButtonCenter->setEnabled(false);
	}
	prepareMap();
}

MapEditor::~MapEditor()
{
	delete ui;
	ui=nullptr;
}


void MapEditor::prepareMap(){
	qmapcontrol::MapControl *mc=ui->widgetMap;
	if(0!=mc){
		mc->showScale(true);
		QDir dir("./map.cache/");
		if (!dir.exists()) dir.mkpath(".");
		mc->enablePersistentCache ( dir,8192);
		// create MapAdapter to get maps from
		//		qmapcontrol::MapAdapter* mapadapter = OC_NEW qmapcontrol::OSMMapAdapter();
		//qmapcontrol::MapAdapter* mapadapter = OC_NEW qmapcontrol::OpenAerialMapAdapter();
		//qmapcontrol::TileMapAdapter* mapadapter = OC_NEW qmapcontrol::TileMapAdapter("tile.openstreetmap.org", "/%1/%2/%3.png", 256, 0, 17);
		qmapcontrol::TileMapAdapter* mapadapter = OC_NEW qmapcontrol::GoogleMapAdapter(qmapcontrol::GoogleMapAdapter::satellite);



		//qmapcontrol::TileMapAdapter* mapadapter = OC_NEW qmapcontrol::TileMapAdapter("cache.kartverket.no/grunnkart/wmts", "/%1/%2/%3.png", 256, 0, 17);

		// create a map layer with the mapadapter
		qmapcontrol::Layer* l = OC_NEW qmapcontrol::MapLayer("Custom Layer", mapadapter);
		// add Layer to the MapControl
		mc->addLayer(l);

		// create a LineString
		QList<qmapcontrol::Point*> points;

		points.append(OC_NEW qmapcontrol::Point( 5.456635,60.384571, "HQ"));
		points.append(OC_NEW qmapcontrol::Point(  5.456249,60.384317, "Launch point"));

		// A QPen also can use transparency
		QPen* linepen = OC_NEW QPen(QColor(0, 0, 255, 100));
		linepen->setWidth(5);
		// Add the Points and the QPen to a LineString
		qmapcontrol::LineString* ls = OC_NEW qmapcontrol::LineString(points, "Launch vector", linepen);

		// Add the LineString to the layer
		l->addGeometry(ls);
		homeMap();
	}
}





void MapEditor::homeMap() {
	qmapcontrol::MapControl *mc=ui->widgetMap;
	if(0!=mc){
		//qDebug()<<"HOME";
		QList<QPointF> londalen;
		londalen <<QPointF(5.452844, 60.385883);
		londalen <<QPointF(5.457945, 60.380353);
		mc->setViewAndZoomIn ( londalen) ;
	}
}


void MapEditor::onPositionUpdated(QGeoPositionInfo pi){
	if(pi.isValid() ){
		QGeoCoordinate c=pi.coordinate();
		if(c.isValid()){
			qmapcontrol::MapControl *mc=ui->widgetMap;
			if(0!=mc){
				QPointF p(c.longitude(), c.latitude());
				//qDebug()<<"CENTER";
				QList<QPointF> center;
				center <<p;
				center <<p;
				mc->setViewAndZoomIn ( center) ;
			}
		}
	}
}

void MapEditor::on_toolButtonHome_clicked(){
	ui->toolButtonCenter->setChecked(false);
	homeMap();
}

void MapEditor::on_toolButtonCenter_toggled(bool checked)
{
	if(checked){
		gps->startUpdates();
	}
	else{
		gps->stopUpdates();
	}
}
