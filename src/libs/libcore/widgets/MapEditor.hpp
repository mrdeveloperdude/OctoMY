#ifndef MAPEDITOR_HPP
#define MAPEDITOR_HPP


#include "map/layers/MapLayer.hpp"

#include <QWidget>
#include <QGeoPositionInfo>

#include <QSharedPointer>
#include <QStandardPaths>

namespace Ui
{
class MapEditor;
}


/*

const QString cacheDir=QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
if(""!=cacheDir){
	QDir dir(cacheDir);
	if(!dir.exists()){
		if(!dir.mkpath(cacheDir)){
			qWarning()<<"ERROR: Could not create HTTP cache dir "<<cacheDir;
		}
	}
	if (dir.exists()){
		QNetworkDiskCache *cache=OC_NEW QNetworkDiskCache;
		cache->setCacheDirectory(dir.absoluteFilePath("NetworkHorse"));
		nam->setCache(cache);
	}

	*/




struct MapType {
	QString name;
	QString cacheDir;
	QSharedPointer<qmapcontrol::TileMapAdapter> mapAdapter;
	quint64 persistentCacheMiB;

	explicit MapType(QString name="", QSharedPointer<qmapcontrol::TileMapAdapter> mapAdapter=nullptr, quint64 persistentCacheMiB=48)
		: name(name)
		, cacheDir(QStandardPaths::writableLocation(QStandardPaths::CacheLocation)+"/map.cache."+name+"/")
		, mapAdapter(mapAdapter)
		, persistentCacheMiB(persistentCacheMiB)
	{
		QDir dir(cacheDir);
		if (!dir.exists()) {
			dir.mkpath(".");
		}
		qDebug()<<"Map tiles for "<<name<<" is kept in cache dir "<<dir.absolutePath();
	}

	virtual ~MapType(){
	}
};



class QGeoPositionInfoSource;

class MapEditor : public QWidget
{
	Q_OBJECT

private:
	Ui::MapEditor *ui;
	QGeoPositionInfoSource *mGps;
	QMap<QString, MapType> mMapTypes;
	QSharedPointer<qmapcontrol::Layer> mMapLayer;

public:
	explicit MapEditor(QWidget *parent = 0);
	~MapEditor();


private:

	void registerMapType(MapType mapType);
	void prepareMapTypes();
	void selectMapType(QString name);

public:


	void prepareMap();
	void homeMap();

private slots:
	void onPositionUpdated(QGeoPositionInfo);
	void onMapControlViewChanged(const QPointF &coordinate, int zoom );
	void on_toolButtonHome_clicked();
	void on_toolButtonCenter_toggled(bool checked);
	void on_comboBoxMapType_currentIndexChanged(int index);
};

#endif // MAPEDITOR_HPP
