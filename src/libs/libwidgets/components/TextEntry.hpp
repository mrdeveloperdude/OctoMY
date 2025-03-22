#ifndef TEXTENTRY_HPP
#define TEXTENTRY_HPP

#include "uptime/SharedPointerWrapper.hpp"

#include <QLineEdit>

class Settings;

class TextEntry : public QLineEdit
{
	Q_OBJECT
private:
	QString k;
	QSharedPointer<Settings> settings;
public:
	explicit TextEntry(QWidget *parent = nullptr);
	virtual ~TextEntry();
public:
	void configure(QSharedPointer<Settings> settings, QString val, QString key="", QString tooltip="");

private slots:
	void onValueChanged();


};

#endif
// TEXTENTRY_HPP
