#ifndef IDENTICON_HPP
#define IDENTICON_HPP

#include "security/PortableID.hpp"

#include <QtGlobal>
#include <QDomDocument>
#include <QtGui/QPixmap>
#include <QByteArray>

class RNG;

class Identicon{

	private:
		QByteArray data;
		QString url;
		bool dirty;
		PortableID mID;
		QDomDocument doc;
		RNG *rng;

	public:
		explicit Identicon(QString url, PortableID &id);
		explicit Identicon(QString url="");
		virtual  ~Identicon();

	private:

		float frand();

	public:
		void setSvgURL(QString);
		void setPortableID(PortableID id);

		QDomDocument domDocument();

		QPixmap pixmap(qint32 w=-1,qint32 h=-1,qreal zoom=0.0);
		QImage image(qint32 w=-1,qint32 h=-1,qreal zoom=0.0);

	private:

		void regenerateIdenticon();
};

#endif // IDENTICON_HPP
