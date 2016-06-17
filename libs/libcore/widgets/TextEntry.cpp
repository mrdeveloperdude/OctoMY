#include "TextEntry.hpp"
#include "basic/Standard.hpp"
#include "basic/Settings.hpp"

TextEntry::TextEntry(QWidget *parent) :
	QLineEdit(parent)
{
	OC_METHODGATE();
}

TextEntry::~TextEntry(){
	OC_METHODGATE();
}



void TextEntry::configure(Settings *s,QString val, QString key, QString tip){
	OC_METHODGATE();
	settings=s;
	if(""!=tip){
		setToolTip(tip);
	}
	k=key.trimmed();
	if(""!=k && nullptr!=settings){
		val=settings->getCustomSetting(k,val);
		if(!connect(this,SIGNAL(textChanged(QString)),this,SLOT(onValueChanged()),OC_CONTYPE)){
			qWarning()<<"ERROR: Could not connect";
		}
	}
	setText(val);
}


void TextEntry::onValueChanged(){
	OC_METHODGATE();
	QString v=text();
	if(nullptr!=settings){
		settings->setCustomSetting(k,v);
	}
}
