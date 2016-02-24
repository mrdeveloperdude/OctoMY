#include "LocalAddressEntry.hpp"

#include "basic/Standard.hpp"
#include "basic/Settings.hpp"
#include "utility/Utility.hpp"

LocalAddressEntry::LocalAddressEntry(QWidget *parent)
	: QComboBox(parent)
{
	utility::populateComboboxWithLocalAdresses(*this);
	defaultAddress=currentText();
	if(!connect(this,SIGNAL(currentTextChanged(QString)),this,SLOT(onValueChanged(QString)),WWCONTYPE)){
		qWarning()<<"ERROR: Could not connect";
	}
}

LocalAddressEntry::~LocalAddressEntry(){

}

void LocalAddressEntry::configure(QString key, QString tip){
	WWMETHODGATE();
	if(""!=tip){
		setToolTip(tip);
	}
	k=key.trimmed();
	QString val=defaultAddress;
	if(""!=k){
		//qDebug()<<"Default address: "<<val;
		val=Settings::getInstance().getCustomSetting(k,val);
	}
	//qDebug()<<"Setting current address: "<<val;
	setCurrentText(val);
}


void LocalAddressEntry::onValueChanged(QString v){
	WWMETHODGATE();
	Settings::getInstance().setCustomSetting(k,v);
}
