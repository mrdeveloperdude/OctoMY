#ifndef IDENTICON_HPP
#define IDENTICON_HPP

#include <QtGlobal>
#include <QDomDocument>
#include <QtGui/QPixmap>
#include <QByteArray>

class Identicon{
	private:
		QByteArray data;
		QString url;
		bool dirty;
		QByteArray ba;
		QDomDocument doc;
	public:
		explicit Identicon(QString url="", QByteArray data=QByteArray());
		virtual  ~Identicon();

	public:
		void setSvgURL(QString);
		void setIdenticonData(QByteArray);

		QDomDocument domDocument();

		QPixmap pixmap(qint32 w=-1,qint32 h=-1,qreal zoom=0.0);
		QImage image(qint32 w=-1,qint32 h=-1,qreal zoom=0.0);

	private:

		void regenerateIdenticon();
};

#endif // IDENTICON_HPP
