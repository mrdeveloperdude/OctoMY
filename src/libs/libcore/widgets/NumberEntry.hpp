#ifndef NUMBERENTRY_HPP
#define NUMBERENTRY_HPP

#include <QWidget>

namespace Ui {
	class NumberEntry;
}

class Settings;

class NumberEntry : public QWidget{
		Q_OBJECT
	private:
		Ui::NumberEntry *ui;
		QString k;
		Settings *settings;
	public:
		explicit NumberEntry(QWidget *parent = 0);
		~NumberEntry();

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

#endif // NUMBERENTRY_HPP

