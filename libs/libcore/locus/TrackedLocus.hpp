#ifndef TRACKEDLOCUS_HPP
#define TRACKEDLOCUS_HPP

#include "Locus.hpp"

class TrackedLocusWidget;


class TrackedLocus : public Locus
{


private:

	TrackedLocusWidget *mConfigWidget;
public:
	TrackedLocus();


	// Locus interface
public:
	QWidget *configurationWidget() Q_DECL_OVERRIDE;
	QVariantMap configuration() Q_DECL_OVERRIDE;
	void setConfiguration(QVariantMap &configuration) Q_DECL_OVERRIDE;


};

#endif // TRACKEDLOCUS_HPP
