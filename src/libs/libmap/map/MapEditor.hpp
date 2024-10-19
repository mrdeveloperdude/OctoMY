#ifndef MAPEDITOR_HPP
#define MAPEDITOR_HPP

#include "uptime/SharedPointerWrapper.hpp"

#include <QDir>
#include <QGeoPositionInfo>
#include <QStandardPaths>
#include <QWidget>

namespace qmapcontrol{
class TileMapAdapter;
class Layer;
}

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
	bool doDebug{false};

	explicit MapType(QString name="", QSharedPointer<qmapcontrol::TileMapAdapter> mapAdapter=nullptr, quint64 persistentCacheMiB=48, bool doDebug=false)
		: name(name)
		, cacheDir(QStandardPaths::writableLocation(QStandardPaths::CacheLocation)+"/map.cache."+name+"/")
		, mapAdapter(mapAdapter)
		, persistentCacheMiB(persistentCacheMiB)
		, doDebug(doDebug)
	{
		QDir dir(cacheDir);
		if (!dir.exists()) {
			dir.mkpath(".");
		}
		if(doDebug){
			qDebug()<<"Map tiles for "<<name<<" is kept in cache dir "<<dir.absolutePath();
		}
	}

	virtual ~MapType()
	{
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
	bool mDebug{false};

public:
	explicit MapEditor(QWidget *parent = nullptr);
	~MapEditor();

private:
	void registerMapType(MapType mapType);
	void prepareMapTypes();
	void selectMapType(QString name);
	void prepareLocationEditor();

public:
	void prepareMap();
	
private slots:
	void onPositionUpdated(QGeoPositionInfo);
	void onMapControlViewChanged(const QPointF &coordinate, int zoom );
	void onLocationEditorDone(bool done);
	
public slots:
	void homeMap();
	void toggleCenter(bool checked);
	void setMapType(int index);
	void createLocation();

};

#endif
// MAPEDITOR_HPP
