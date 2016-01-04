#include "TextEntry.hpp"
#include "basic/Standard.hpp"
#include "basic/Settings.hpp"

TextEntry::TextEntry(QWidget *parent) :
	QLineEdit(parent)
{
	WWMETHODGATE();
}

TextEntry::~TextEntry(){
	WWMETHODGATE();
}



void TextEntry::configure(QString val, QString key, QString tip){
	WWMETHODGATE();
	if(""!=tip){
		setToolTip(tip);
	}
	k=key.trimmed();
	if(""!=k){
		val=Settings::getInstance().getCustomSetting(k,val);
		if(!connect(this,SIGNAL(textChanged(QString)),this,SLOT(onValueChanged()),WWCONTYPE)){
			qWarning()<<"ERROR: Could not connect";
		}
	}
	setText(val);
}


void TextEntry::onValueChanged(){
	WWMETHODGATE();
	QString v=text();
	Settings::getInstance().setCustomSetting(k,v);
}
