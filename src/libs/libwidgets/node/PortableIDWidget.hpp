#ifndef PORTABLEIDWIDGET_HPP
#define PORTABLEIDWIDGET_HPP

#include "security/PortableID.hpp"
#include "uptime/ConfigureHelper.hpp"

#include <QWidget>

namespace Ui
{
class PortableIDWidget;
}

class Activity;

class PortableIDWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::PortableIDWidget *ui;
	PortableID mID;
	bool mInlineCertificate{true};
	Activity *mExternalCertificateActivity{nullptr};
	bool mDebug{false};
	ConfigureHelper mConfigureHelper;

public:
	explicit PortableIDWidget(QWidget *parent = nullptr);
	virtual ~PortableIDWidget();

public:
	// If the activity is set we lean on the separate identity activity for showing the certificate rather than using the internal display
	void configure(Activity *externalCertificateActivity, bool showCertificateFirst, bool userCanChange);
	void setPortableID(PortableID id);
	PortableID getPortableID();

private slots:
	void showCertificate();
	void showNameplate();

};

#endif
// PORTABLEIDWIDGET_HPP
