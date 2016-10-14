#include "EnumEntry.hpp"
#include "../libutil/utility/Standard.hpp"
#include "basic/Settings.hpp"

EnumEntry::EnumEntry(QWidget *parent)
	: QComboBox(parent)
	, settings(nullptr)
{
	OC_METHODGATE();
}

EnumEntry::~EnumEntry(){
	OC_METHODGATE();
}



void EnumEntry::configure(Settings &s, QString val, QString key, QString tip){
	OC_METHODGATE();
	settings=&s;
	if(""!=tip){
		setToolTip(tip);
	}
	k=key.trimmed();
	if(""!=k){
		val=settings->getCustomSetting(k,val);
		if(!connect(this,SIGNAL(currentTextChanged(QString)),this,SLOT(onValueChanged(QString)),OC_CONTYPE)){
			qWarning()<<"ERROR: Could not connect";
		}
	}
	setCurrentText(val);
}


void EnumEntry::onValueChanged(QString v){
	OC_METHODGATE();
	if(nullptr!=settings){
		settings->setCustomSetting(k,v);
	}
}
