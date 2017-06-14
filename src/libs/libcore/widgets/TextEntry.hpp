#ifndef TEXTENTRY_HPP
#define TEXTENTRY_HPP

#include <QLineEdit>


class Settings;

class TextEntry : public QLineEdit{
		Q_OBJECT
	private:
		QString k;
		Settings *settings;
	public:
		explicit TextEntry(QWidget *parent = 0);
		virtual ~TextEntry();
	public:
		void configure(Settings *settings, QString val, QString key="", QString tooltip="");

	private slots:
		void onValueChanged();


};

#endif // TEXTENTRY_HPP
