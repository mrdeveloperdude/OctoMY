#ifndef WHEELEDLobe_HPP
#define WHEELEDLobe_HPP

#include "Lobe.hpp"

class WheeledLobeWidget;

class WheeledLobe : public Lobe
{
private:
	WheeledLobeWidget *mConfigWidget;

public:
	WheeledLobe(QObject *parent = nullptr);

	// Lobe interface
public:
	QWidget *configurationWidget() override;
	QVariantMap configuration() override;
	void setConfiguration(QVariantMap &configuration) override;

};

#endif
// WHEELEDLobe_HPP
