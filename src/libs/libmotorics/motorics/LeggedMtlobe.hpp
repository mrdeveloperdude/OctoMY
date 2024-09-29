#ifndef LEGGEDMTLOBE_HPP
#define LEGGEDMTLOBE_HPP
#include "Mtlobe.hpp"

class LeggedMtlobeWidget;


class LeggedMtlobe : public Mtlobe
{

private:
	LeggedMtlobeWidget *mConfigWidget;

public:
	LeggedMtlobe();

	// Mtlobe interface
public:
	QWidget *configurationWidget() override;
	QVariantMap configuration() override;
	void setConfiguration(QVariantMap &configuration) override;

};

#endif
// LEGGEDMTLOBE_HPP
