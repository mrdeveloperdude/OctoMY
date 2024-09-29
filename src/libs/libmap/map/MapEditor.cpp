#include "MapEditor.hpp"
#include "ui_MapEditor.h"


#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "uptime/ConnectionType.hpp"

#include "map/MapControl.hpp"
#include "map/layers/MapLayer.hpp"
#include "map/geometries/curves/LineString.hpp"
#include "map/adapters/OSMMapAdapter.hpp"
#include "map/adapters/OpenAerialMapAdapter.hpp"
#include "map/adapters/GoogleMapAdapter.hpp"
#include "MapLocationEditor.hpp"

#include <QGeoPositionInfoSource>



class MapGeometryFactory
{



};


MapEditor::MapEditor(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::MapEditor)
	, mGps(QGeoPositionInfoSource::createDefaultSource(this))
	, mMapLayer(nullptr)
{
	OC_METHODGATE();
	ui->setupUi(this);
	if (nullptr != mGps) {
		mGps->setPreferredPositioningMethods(QGeoPositionInfoSource::SatellitePositioningMethods);
		if(nullptr != mGps) {
			if(!connect(mGps, SIGNAL(positionUpdated(QGeoPositionInfo)),this,SLOT(onPositionUpdated(QGeoPositionInfo)))) {
				qWarning()<<"ERROR: Could not connect";
			}
		}
	} else {
		ui->toolButtonCenter->setEnabled(false);
	}
	prepareMapTypes();
	prepareMap();
	prepareLocationEditor();
	selectMapType("google.roadmap");
	homeMap();
}

MapEditor::~MapEditor()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}



void MapEditor::registerMapType(MapType mapType)
{
	OC_METHODGATE();
	mMapTypes[mapType.name]=mapType;
}


void MapEditor::prepareMapTypes()
{
	OC_METHODGATE();
	mMapTypes.clear();
	registerMapType(MapType("google.roadmap", QSharedPointer<qmapcontrol::GoogleMapAdapter>::create(qmapcontrol::GoogleMapAdapter::roadmap)));
	registerMapType(MapType("google.satellite", QSharedPointer<qmapcontrol::GoogleMapAdapter>::create(qmapcontrol::GoogleMapAdapter::satellite)));
	registerMapType(MapType("google.terrain", QSharedPointer<qmapcontrol::GoogleMapAdapter>::create(qmapcontrol::GoogleMapAdapter::terrain)));
	registerMapType(MapType("google.hybrid", QSharedPointer<qmapcontrol::GoogleMapAdapter>::create(qmapcontrol::GoogleMapAdapter::hybrid)));
	registerMapType(MapType("openstreetmap.default", QSharedPointer<qmapcontrol::TileMapAdapter>::create("tile.openstreetmap.org", "/%1/%2/%3.png", 256, 0, 17)));
	registerMapType(MapType("kartverket.grunnkart", QSharedPointer<qmapcontrol::TileMapAdapter>::create("cache.kartverket.no/grunnkart/wmts", "/%1/%2/%3.png", 256, 0, 17)));
	//		qmapcontrol::MapAdapter* mapadapter = OC_NEW qmapcontrol::OSMMapAdapter();
	//qmapcontrol::MapAdapter* mapadapter = OC_NEW qmapcontrol::OpenAerialMapAdapter();
	//qmapcontrol::TileMapAdapter* mapadapter = OC_NEW qmapcontrol::TileMapAdapter("tile.openstreetmap.org", "/%1/%2/%3.png", 256, 0, 17);
	//qmapcontrol::TileMapAdapter* mapadapter = OC_NEW qmapcontrol::TileMapAdapter("cache.kartverket.no/grunnkart/wmts", "/%1/%2/%3.png", 256, 0, 17);

	QString selected=ui->comboBoxMapType->currentText();
	ui->comboBoxMapType->clear();
	ui->comboBoxMapType->addItems(mMapTypes.keys());
	ui->comboBoxMapType->setCurrentText(selected);
}


void MapEditor::selectMapType(QString name)
{
	OC_METHODGATE();
	qmapcontrol::MapControl *mc=ui->widgetMap;
	if(nullptr!=mc) {
		if(mMapTypes.contains(name)) {
			MapType &mt=mMapTypes[name];
			QDir dir(mt.cacheDir);
			if (!dir.exists()) {
				dir.mkpath(".");
			}
			mc->enablePersistentCache ( dir, static_cast<int>(mt.persistentCacheMiB));
			if(!mMapLayer.isNull()) {
				qDebug()<<"Setting new map type: "<<name;
				mc->removeLayer(mMapLayer);
				mMapLayer->setMapAdapter(mt.mapAdapter);
				mc->addLayer(mMapLayer);
			}
		}
	}
}



void MapEditor::prepareLocationEditor()
{
	OC_METHODGATE();
	if(!connect(ui->widgetLocationEditor, &MapLocationEditor::done, this, &MapEditor::onLocationEditorDone, OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect to "<<ui->widgetLocationEditor->objectName();
	}
}


void MapEditor::prepareMap()
{
	OC_METHODGATE();
	qmapcontrol::MapControl *mc=ui->widgetMap;
	if(nullptr!=mc) {
		mc->showScale(true);
		// Create a map layer with the mapadapter
		mMapLayer = QSharedPointer<qmapcontrol::Layer>::create(QString("Map Layer"), nullptr);
		if(nullptr!=mMapLayer) {
			// Add Layer to the MapControl
			mc->addLayer(mMapLayer);
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
			mMapLayer->addGeometry(ls);
		} else {
			qWarning()<<"ERROR: null was mMapLayer";
		}
		connect(mc, &qmapcontrol::MapControl::viewChanged, this, &MapEditor::onMapControlViewChanged, OC_CONTYPE);
	}
}





void MapEditor::homeMap()
{
	OC_METHODGATE();
	qmapcontrol::MapControl *mc=ui->widgetMap;
	if(nullptr != mc) {
		//qDebug()<<"HOME";
		QList<QPointF> londalen;
		londalen <<QPointF(5.452844, 60.385883);
		londalen <<QPointF(5.457945, 60.380353);
		mc->setViewAndZoomIn ( londalen) ;
	}
}


void MapEditor::onPositionUpdated(QGeoPositionInfo pi)
{
	OC_METHODGATE();
	if(pi.isValid() ) {
		QGeoCoordinate c=pi.coordinate();
		if(c.isValid()) {
			qmapcontrol::MapControl *mc=ui->widgetMap;
			if(nullptr != mc) {
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

void MapEditor::onMapControlViewChanged(const QPointF &coordinate, int zoom )
{
	OC_METHODGATE();
	QString str=QString("lat-long: <%1, %2> zoom: %3").arg(coordinate.y()).arg(coordinate.x()).arg(zoom);
	ui->labelPosition->setText(str);
}



void MapEditor::onLocationEditorDone(bool done)
{
	OC_METHODGATE();
	qDebug()<<"DONE: "<<done;
	ui->stackedWidget->setCurrentWidget(ui->pageMapEditor);
}

void MapEditor::on_toolButtonHome_clicked()
{
	OC_METHODGATE();
	ui->toolButtonCenter->setChecked(false);
	homeMap();
}

void MapEditor::on_toolButtonCenter_toggled(bool checked)
{
	OC_METHODGATE();
	if(checked) {
		mGps->startUpdates();
	} else {
		mGps->stopUpdates();
	}
}

void MapEditor::on_comboBoxMapType_currentIndexChanged(int)
{
	OC_METHODGATE();
	selectMapType(ui->comboBoxMapType->currentText());
}

void MapEditor::on_pushButtonCreateLocation_clicked()
{
	OC_METHODGATE();
	ui->stackedWidget->setCurrentWidget(ui->pageLocationEditor);
}
