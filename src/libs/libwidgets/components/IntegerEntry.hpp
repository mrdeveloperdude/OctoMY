#ifndef INTEGERENTRY_HPP
#define INTEGERENTRY_HPP

#include <QWidget>

namespace Ui
{
class IntegerEntry;
}

class Settings;

class IntegerEntry : public QWidget
{
	Q_OBJECT
private:
	Ui::IntegerEntry *ui;
	QString k;
	Settings *settings;
public:
	explicit IntegerEntry(QWidget *parent = nullptr);
	~IntegerEntry();

public:
	void configure(Settings *settings, int min, int max, int step, int val, QString suf, QString tooltip="", QString key="");

	int value();
	int minimum();
	int maximum();
	void reloadData();

private slots:
	void setValueInternal(int val);
public slots:
	void setValue(int val);
signals:
	void valueChanged(int v);

};

#endif // INTEGERENTRY_HPP

