#ifndef LOCUSCONTROLLER_HPP
#define LOCUSCONTROLLER_HPP

#include "LocusSet.hpp"
#include "WheeledLocus.hpp"

#include <QObject>

class LocusController: public QObject
{
	Q_OBJECT
private:
	LocusSet mLoci;
public:
	LocusController(QObject *parent=nullptr)
		: QObject(parent)
	{

	}

	Locus *addLocus()
	{
		Locus *l=new WheeledLocus();
		mLoci.push_back(l);
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
