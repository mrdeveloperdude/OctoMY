#ifndef HOVERINGLobe_HPP
#define HOVERINGLobe_HPP

#include "Lobe.hpp"

class HoveringLobeWidget;


class HoveringLobe : public Lobe
{

private:
	HoveringLobeWidget *mConfigWidget;

public:
	HoveringLobe(QObject *parent = nullptr);

	// Lobe interface
public:
	QWidget *configurationWidget() override;
	QVariantMap configuration() override;
	void setConfiguration(QVariantMap &configuration) override;

};


#endif
// HOVERINGLobe_HPP
