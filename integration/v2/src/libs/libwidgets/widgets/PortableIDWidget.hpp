#ifndef PORTABLEIDWIDGET_HPP
#define PORTABLEIDWIDGET_HPP

#include "security/PortableID.hpp"
#include "uptime/ConfigureHelper.hpp"

#include <QWidget>

namespace Ui
{
class PortableIDWidget;
}

class PortableIDWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::PortableIDWidget *ui;
	PortableID mID;
	ConfigureHelper mConfigureHelper;

public:
	explicit PortableIDWidget(QWidget *parent = nullptr);
	virtual ~PortableIDWidget();

public:
	void configure(bool showCertificateFirst=true, bool userCanChange=true);
	void setPortableID(PortableID id);
	PortableID getPortableID();

private slots:
	void on_pushButtonToggleView_toggled(bool checked);
};

#endif
// PORTABLEIDWIDGET_HPP
