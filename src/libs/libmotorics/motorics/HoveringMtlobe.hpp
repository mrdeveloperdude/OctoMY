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
	QWidget *configurationWidget() override;
	QVariantMap configuration() override;
	void setConfiguration(QVariantMap &configuration) override;

};


#endif
// HOVERINGMTLOBE_HPP
