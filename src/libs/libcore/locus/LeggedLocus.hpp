#ifndef LEGGEDLOCUS_HPP
#define LEGGEDLOCUS_HPP
#include "Locus.hpp"

class LeggedLocusWidget;


class LeggedLocus : public Locus
{


private:

	LeggedLocusWidget *mConfigWidget;
public:
	LeggedLocus();


	// Locus interface
public:
	QWidget *configurationWidget() Q_DECL_OVERRIDE;
	QVariantMap configuration() Q_DECL_OVERRIDE;
	void setConfiguration(QVariantMap &configuration) Q_DECL_OVERRIDE;


};

#endif // LEGGEDLOCUS_HPP
