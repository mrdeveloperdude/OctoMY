#ifndef HOVERINGMTLOBE_HPP
#define HOVERINGMTLOBE_HPP

#include "Mtlobe.hpp"

class HoveringMtlobeWidget;


class HoveringMtlobe : public Mtlobe
{

private:
	HoveringMtlobeWidget *mConfigWidget;

public:
	HoveringMtlobe();

	// Mtlobe interface
public:
	QWidget *configurationWidget() Q_DECL_OVERRIDE;
	QVariantMap configuration() Q_DECL_OVERRIDE;
	void setConfiguration(QVariantMap &configuration) Q_DECL_OVERRIDE;

};


#endif
// HOVERINGMTLOBE_HPP
