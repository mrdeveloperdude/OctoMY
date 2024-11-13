#ifndef TRACKEDLobe_HPP
#define TRACKEDLobe_HPP

#include "Lobe.hpp"

class TrackedLobeWidget;


class TrackedLobe : public Lobe
{

private:
	TrackedLobeWidget *mConfigWidget;
public:
	TrackedLobe(QObject *parent = nullptr);

	// Lobe interface
public:
	QWidget *configurationWidget() override;
	QVariantMap configuration() override;
	void setConfiguration(QVariantMap &configuration) override;

};

#endif
// TRACKEDLobe_HPP
