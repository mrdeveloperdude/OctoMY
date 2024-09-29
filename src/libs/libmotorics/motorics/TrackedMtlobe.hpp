#ifndef TRACKEDMTLOBE_HPP
#define TRACKEDMTLOBE_HPP

#include "Mtlobe.hpp"

class TrackedMtlobeWidget;


class TrackedMtlobe : public Mtlobe
{

private:
	TrackedMtlobeWidget *mConfigWidget;
public:
	TrackedMtlobe();

	// Mtlobe interface
public:
	QWidget *configurationWidget() override;
	QVariantMap configuration() override;
	void setConfiguration(QVariantMap &configuration) override;

};

#endif
// TRACKEDMTLOBE_HPP
