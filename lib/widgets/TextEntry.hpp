#ifndef TEXTENTRY_HPP
#define TEXTENTRY_HPP

#include <QLineEdit>

class TextEntry : public QLineEdit{
		Q_OBJECT
	private:
		QString k;
	public:
		explicit TextEntry(QWidget *parent = 0);
		virtual ~TextEntry();
	public:
		void configure(QString val, QString key="", QString tooltip="");

	private slots:
		void onValueChanged();


};

#endif // TEXTENTRY_HPP
