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
	QWidget *configurationWidget() Q_DECL_OVERRIDE;
	QVariantMap configuration() Q_DECL_OVERRIDE;
	void setConfiguration(QVariantMap &configuration) Q_DECL_OVERRIDE;

};

#endif
// TRACKEDMTLOBE_HPP
