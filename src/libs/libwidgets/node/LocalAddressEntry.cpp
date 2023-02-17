#include "LocalAddressEntry.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/ConnectionType.hpp"


#include "app/Settings.hpp"


#include "utility/ui/Ui.hpp"

#include <QDebug>


LocalAddressEntry::LocalAddressEntry(QWidget *parent)
	: QComboBox(parent)
	, settings(nullptr)
{
	OC_METHODGATE();
	utility::ui::populateComboboxWithLocalAdresses(*this);
	defaultAddress=currentText();
	if(!connect(this, SIGNAL(currentTextChanged(QString)), this, SLOT(onValueChanged(QString)), OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}
}

LocalAddressEntry::~LocalAddressEntry()
{
	OC_METHODGATE();
}

void LocalAddressEntry::configure(Settings *s, QString key, QString tip)
{
	OC_METHODGATE();
	settings=s;
	if(""!=tip) {
		setToolTip(tip);
	}
	k=key.trimmed();
	QString val=defaultAddress;
	if(""!=k && nullptr!=settings) {
		//qDebug()<<"Default address: "<<val;
		val=settings->getCustomSetting(k,val);
	}
	//qDebug()<<"Setting current address: "<<val;
	setCurrentText(val);
}


void LocalAddressEntry::onValueChanged(QString v)
{
	OC_METHODGATE();
	if(nullptr!=settings) {
		settings->setCustomSetting(k,v);
	}
}
