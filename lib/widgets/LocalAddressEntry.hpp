#ifndef LOCALADDRESSENTRY_HPP
#define LOCALADDRESSENTRY_HPP

#include <QComboBox>

class LocalAddressEntry: public QComboBox{
		Q_OBJECT
	private:
		QString defaultAddress;
		QString k;

	public:
		explicit LocalAddressEntry(QWidget *parent=0);
		virtual ~LocalAddressEntry();
	public:
		void configure(QString key, QString tip);
	private slots:
		void onValueChanged(QString);
};

#endif // LOCALADDRESSENTRY_HPP
