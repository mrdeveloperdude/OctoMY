#ifndef LOCUSCONTROLLER_HPP
#define LOCUSCONTROLLER_HPP

#include "LocusSet.hpp"
#include "LocusFactory.hpp"

#include <QObject>

class LocusController: public QObject
{
	Q_OBJECT
private:
	LocusSet mLoci;
	LocusFactory mFactory;
public:
	LocusController(QObject *parent=nullptr);
public:

	Locus *addLocus(QString type);
	LocusSet &loci();
signals:

	void locusConfigurationChanged();
public slots:
	void onLocusWidgetDeleted(quint32);

};

#endif // LOCUSCONTROLLER_HPP
