#include "EnumEntry.hpp"
#include "basic/Standard.hpp"
#include "basic/Settings.hpp"

EnumEntry::EnumEntry(QWidget *parent) :
	QComboBox(parent)
{
	WWMETHODGATE();
}

EnumEntry::~EnumEntry(){
	WWMETHODGATE();
}



void EnumEntry::configure(QString val, QString key, QString tip){
	WWMETHODGATE();
	if(""!=tip){
		setToolTip(tip);
	}
	k=key.trimmed();
	if(""!=k){
		val=Settings::getInstance().getCustomSetting(k,val);
		if(!connect(this,SIGNAL(currentTextChanged(QString)),this,SLOT(onValueChanged(QString)),WWCONTYPE)){
			qWarning()<<"ERROR: Could not connect";
		}
	}
	setCurrentText(val);
}


void EnumEntry::onValueChanged(QString v){
	WWMETHODGATE();
	Settings::getInstance().setCustomSetting(k,v);
}
