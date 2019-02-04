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
	QWidget *configurationWidget() Q_DECL_OVERRIDE;
	QVariantMap configuration() Q_DECL_OVERRIDE;
	void setConfiguration(QVariantMap &configuration) Q_DECL_OVERRIDE;

};

#endif
// LEGGEDMTLOBE_HPP
