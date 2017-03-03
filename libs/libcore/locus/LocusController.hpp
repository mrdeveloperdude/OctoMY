#ifndef LOCUSCONTROLLER_HPP
#define LOCUSCONTROLLER_HPP

#include "LocusSet.hpp"
#include "WheeledLocus.hpp"
#include "LocusFactory.hpp"

#include <QObject>

class LocusController: public QObject
{
	Q_OBJECT
private:
	LocusSet mLoci;
	LocusFactory mFactory;
public:
	LocusController(QObject *parent=nullptr)
		: QObject(parent)
	{

	}

	Locus *addLocus(QString type)
	{
		Locus *l=mFactory.locusFactory(type);
		if(nullptr!=l) {
			mLoci.push_back(l);
		}
		return l;
	}

	LocusSet &loci()
	{
		return mLoci;
	}
signals:

	void locusConfigurationChanged();

};

#endif // LOCUSCONTROLLER_HPP
