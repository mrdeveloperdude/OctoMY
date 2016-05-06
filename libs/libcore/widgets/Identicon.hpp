#ifndef IDENTICON_HPP
#define IDENTICON_HPP

#include <QtGlobal>
#include <QDomDocument>
#include <QtGui/QPixmap>


class Identicon{
	private:
		quint64 data;
		QString url;
		bool dirty;
		QByteArray ba;
		QDomDocument doc;
	public:
		explicit Identicon(QString url="", quint64 data=0);
		virtual  ~Identicon();

	public:
		void setSvgURL(QString);
		void setIdenticonData(quint64);

		QDomDocument domDocument();

		QPixmap pixmap();
		QImage image();

	private:

		void regenerateIdenticon();
};

#endif // IDENTICON_HPP
