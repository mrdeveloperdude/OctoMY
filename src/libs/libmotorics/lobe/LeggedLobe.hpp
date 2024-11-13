#ifndef LEGGEDLobe_HPP
#define LEGGEDLobe_HPP
#include "Lobe.hpp"

class LeggedLobeWidget;


class LeggedLobe : public Lobe
{

private:
	LeggedLobeWidget *mConfigWidget;

public:
	LeggedLobe(QObject *parent = nullptr);

	// Lobe interface
public:
	QWidget *configurationWidget() override;
	QVariantMap configuration() override;
	void setConfiguration(QVariantMap &configuration) override;

};

#endif
// LEGGEDLobe_HPP
