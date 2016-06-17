#ifndef ENUMENTRY_HPP
#define ENUMENTRY_HPP

#include <QWidget>
#include <QComboBox>
class Settings;

class EnumEntry : public QComboBox{
		Q_OBJECT
	private:
		QString k;
		Settings *settings;
	public:
		explicit EnumEntry(QWidget *parent = 0);
		virtual ~EnumEntry();
	public:
		void configure(Settings &settings, QString val, QString key="", QString tooltip="");

	private slots:
		void onValueChanged(QString);

};

#endif // ENUMENTRY_HPP
