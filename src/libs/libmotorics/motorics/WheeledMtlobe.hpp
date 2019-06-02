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
	QWidget *configurationWidget() Q_DECL_OVERRIDE;
	QVariantMap configuration() Q_DECL_OVERRIDE;
	void setConfiguration(QVariantMap &configuration) Q_DECL_OVERRIDE;

};

#endif
// WHEELEDMTLOBE_HPP
