#ifndef WHEELEDLOCUS_HPP
#define WHEELEDLOCUS_HPP

#include "Locus.hpp"

class WheeledLocusWidget;

class WheeledLocus : public Locus
{
	private:

		WheeledLocusWidget *mConfigWidget;
public:
	WheeledLocus();


	// Locus interface
public:
	QWidget *configurationWidget() Q_DECL_OVERRIDE;
	QVariantMap configuration() Q_DECL_OVERRIDE;
	void setConfiguration(QVariantMap &configuration) Q_DECL_OVERRIDE;

};

#endif // WHEELEDLOCUS_HPP
