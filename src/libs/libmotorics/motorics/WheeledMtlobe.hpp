#ifndef WHEELEDMTLOBE_HPP
#define WHEELEDMTLOBE_HPP

#include "Mtlobe.hpp"

class WheeledMtlobeWidget;

class WheeledMtlobe : public Mtlobe
{
private:
	WheeledMtlobeWidget *mConfigWidget;

public:
	WheeledMtlobe();

	// Mtlobe interface
public:
	QWidget *configurationWidget() override;
	QVariantMap configuration() override;
	void setConfiguration(QVariantMap &configuration) override;

};

#endif
// WHEELEDMTLOBE_HPP
