#ifndef MTLOBECONTROLLER_HPP
#define MTLOBECONTROLLER_HPP

#include "MtlobeSet.hpp"
#include "MtlobeFactory.hpp"

#include <QObject>

class MtlobeController: public QObject
{
	Q_OBJECT
private:
	MtlobeSet mLoci;
	MtlobeFactory mFactory;
public:
	MtlobeController(QObject *parent=nullptr);
public:

	Mtlobe *addLobe(QString type);
	MtlobeSet &lobes();
signals:

	void mtlobeConfigurationChanged();
public slots:
	void onMtlobeWidgetDeleted(quint32);

};

#endif // MTLOBECONTROLLER_HPP
