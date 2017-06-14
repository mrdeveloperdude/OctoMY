#ifndef HOVERINGLOCUS_HPP
#define HOVERINGLOCUS_HPP

#include "Locus.hpp"

class HoveringLocusWidget;


class HoveringLocus : public Locus
{


private:

	HoveringLocusWidget *mConfigWidget;
public:
	HoveringLocus();


	// Locus interface
public:
	QWidget *configurationWidget() Q_DECL_OVERRIDE;
	QVariantMap configuration() Q_DECL_OVERRIDE;
	void setConfiguration(QVariantMap &configuration) Q_DECL_OVERRIDE;


};


#endif // HOVERINGLOCUS_HPP
