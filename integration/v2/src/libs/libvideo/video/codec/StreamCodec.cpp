#include "StreamCodec.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include "utility/string/String.hpp"

#include <QDataStream>
#include <QSize>
#include <QDebug>
#include <QImage>
#include <QPainter>



////////////////////////////////////////////////////////////////////////


static const auto fmt=QImage::Format_ARGB32;
static const int px=16;

StreamCodec::StreamCodec()
{
	OC_METHODGATE();
}

static quint32 avgColor(const QImage &img)
{
	OC_FUNCTIONGATE();
	int r=0,g=0,b=0, a=0;
	const int w=img.width();
	const int h=img.height();
	const int m=w*h;
	for(int y=0; y<h; ++y) {
		for(int x=0; x<w; ++x) {
			QRgb rgb=img.pixel(x,y);
			r+=qRed(rgb);
			g+=qGreen(rgb);
			b+=qBlue(rgb);
			a+=qAlpha(rgb);
		}
	}
	if(m>0) {
		r/=m;
		g/=m;
		b/=m;
		a/=m;
	}
	return qRgba(r,g,b,a);
}


static quint32 avgColor(const QImage &img, const QRect &rect)
{
	OC_FUNCTIONGATE();
	int r=0,g=0,b=0, a=0;
	const int w=rect.width();
	const int h=rect.height();
	const int m=w*h;
	const int x1=rect.x();
	const int y1=rect.y();
	for(int y=0; y<h; ++y) {
		for(int x=0; x<w; ++x) {
			QRgb rgb=img.pixel(x1+x,y1+y);
			r+=qRed(rgb);
			g+=qGreen(rgb);
			b+=qBlue(rgb);
			a+=qAlpha(rgb);
		}
	}
	if(m>0) {
		r/=m;
		g/=m;
		b/=m;
		a/=m;
	}
	return qRgba(r,g,b,a);
}


struct PixelData {
	QRgb avg;
	QRgb min;
	QRgb max;

	PixelData(QRgb c=0)
		: avg(c)
		, min(c)
		, max(c)
	{
		OC_METHODGATE();
	}

	void set(PixelData &a1, PixelData &a2, PixelData &b1, PixelData &b2)
	{
		OC_METHODGATE();
		avg=(a1.avg + a2.avg + b1.avg + b2.avg)/2;
		min=a1.min;
		min=qMin(min, a2.min);
		min=qMin(min, b1.min);
		min=qMin(min, b2.min);
		max=a1.max;
		max=qMin(max, a2.max);
		max=qMin(max, b1.max);
		max=qMin(max, b2.max);
	}

	QDataStream &toDataStream(QDataStream &ds)
	{
		OC_METHODGATE();
		ds << avg;
		ds << min;
		ds << max;
		return ds;
	}

	QDataStream &fromDataStream(QDataStream &ds)
	{
		OC_METHODGATE();
		ds >> avg;
		ds >> min;
		ds >> max;
		return ds;
	}

};





static QDataStream &operator<<(QDataStream &ds, PixelData &bb)
{
	OC_FUNCTIONGATE();
	return bb.toDataStream(ds);
}

static QDataStream &operator>>(QDataStream &ds, PixelData &bb)
{
	OC_FUNCTIONGATE();
	return bb.fromDataStream(ds);
}


struct PixelNode;

static QDataStream &operator<<(QDataStream &ds, PixelNode &bb);
static QDataStream &operator>>(QDataStream &ds, PixelNode &bb);

static int rgb2lum(QRgb rgb)
{
	OC_FUNCTIONGATE();
	const int r=qRed(rgb)*29;
	const int g=qRed(rgb)*11;
	const int b=qRed(rgb)*59;
	const int ret= (r+g+b)/100;
	return ret;
}

struct PixelNode {
	PixelNode *parent;
	PixelNode *a1, *a2, *b1, *b2;
	PixelData data;

	PixelNode(PixelNode *parent=nullptr, PixelNode *a1=nullptr, PixelNode *a2=nullptr, PixelNode *b1=nullptr, PixelNode *b2=nullptr)
		: parent(parent)
		, a1(a1)
		, a2(a2)
		, b1(b1)
		, b2(b2)
	{
		OC_METHODGATE();

	}

	void free()
	{
		OC_METHODGATE();
#define PIXEL_NODE_FREE(a) if(nullptr!=a){ a->free(); delete a; a=nullptr; }
		PIXEL_NODE_FREE(a1);
		PIXEL_NODE_FREE(a2);
		PIXEL_NODE_FREE(b1);
		PIXEL_NODE_FREE(b2);
#undef PIXEL_NODE_FREE
	}

	void set(PixelNode &a1, PixelNode &a2, PixelNode &b1, PixelNode &b2)
	{
		OC_METHODGATE();
		this->a1=&a1;
		this->a2=&a2;
		this->b1=&b1;
		this->b2=&b2;

		data.set(a1.data, a2.data, b1.data, b2.data);
	}


public:

	QDataStream &toDataStream(QDataStream &ds)
	{
		OC_METHODGATE();
		ds << data;
		const quint8 cc= ((nullptr!=a1)?1:0) | ((nullptr!=a2)?2:0) | ((nullptr!=b1)?4:0) | ((nullptr!=b2)?8:0);
		ds << cc;
		if(nullptr!=a1) {
			PixelNode &t=*a1;
			ds<<t;
		}
		if(nullptr!=a2) {
			ds<<(*a2);
		}
		if(nullptr!=b1) {
			ds<<(*b1);
		}
		if(nullptr!=b2) {
			ds<<(*b2);
		}

		return ds;
	}

	QDataStream &fromDataStream(QDataStream &ds)
	{
		OC_METHODGATE();
		ds >> data;
		quint8 cc=0;
		ds >> cc;
		if(0!=(1&&cc)) {
			a1=OC_NEW PixelNode(this);
			ds>>*a1;
		}
		if(0!=(2&&cc)) {
			a2=OC_NEW PixelNode(this);
			ds>>*a2;
		}
		if(0!=(4&&cc)) {
			b1=OC_NEW PixelNode(this);
			ds>>*b1;
		}
		if(0!=(8&&cc)) {
			b2=OC_NEW PixelNode(this);
			ds>>*b2;
		}
		return ds;
	}


};



static QDataStream &operator<<(QDataStream &ds, PixelNode &bb)
{
	OC_FUNCTIONGATE();
	return bb.toDataStream(ds);
}

static QDataStream &operator>>(QDataStream &ds, PixelNode &bb)
{
	OC_FUNCTIONGATE();
	return bb.fromDataStream(ds);
}






class PixelTree
{
private:
	const int px;
	QImage img;
	QVector<PixelData> nodes;

	PixelNode base;

	QPainter p;
public:
	PixelTree(int px, QImage img)
		: px(px)
		, img(img)
		, nodes(px*px)
	{
		OC_METHODGATE();

	}

	PixelTree(int px)
		: px(px)
		, nodes(px*px)
	{
		OC_METHODGATE();

	}

	void generate()
	{
		OC_METHODGATE();
		base.free();
		generateWorker(base, 0,0,0);
	}

private:

	PixelNode &generateWorker(PixelNode &node, const int x, const int y, int level)
	{
		OC_METHODGATE();
		if(px==level) {
			PixelData me(img.pixel(x,y));
			node.data=me;
			return node;
		} else {
			++level;
			const int nx=x|(1<<level);
			const int ny=y|(1<<level);
			PixelNode &a1=generateWorker(*(OC_NEW PixelNode(node)), x,y, level);
			PixelNode &a2=generateWorker(*(OC_NEW PixelNode(node)), nx,y, level);
			PixelNode &b1=generateWorker(*(OC_NEW PixelNode(node)), x,ny, level);
			PixelNode &b2=generateWorker(*(OC_NEW PixelNode(node)), nx,ny, level);
			node.set(a1, a2, b1, b2);
			return node;
		}
	}

	void reconstructWorker(PixelNode &node, const int x, const int y, int level)
	{
		OC_METHODGATE();
		const int area=(1<<(px-level) );
		p.setBrush(QColor(node.data.avg));
		p.drawRect(QRect(x,y,area,area));
		++level;
		const int nx=x|(1<<level);
		const int ny=y|(1<<level);
		if(nullptr!=node.a1) {
			reconstructWorker(*node.a1, x,y,level);
		}
		if(nullptr!=node.a2) {
			reconstructWorker(*node.a2, nx,y,level);
		}
		if(nullptr!=node.b1) {
			reconstructWorker(*node.b1, x,ny,level);
		}
		if(nullptr!=node.b2) {
			reconstructWorker(*node.b2, nx,ny,level);
		}

	}

public:

	void reconstruct()
	{
		OC_METHODGATE();
		img=QImage(px,px,fmt);
		p.begin(&img);
		p.setPen(Qt::NoPen);
		reconstructWorker(base, 0, 0, 0);
		p.end();
	}

	void draw(QPainter &p)
	{
		OC_METHODGATE();
		reconstruct();
		p.drawImage(0,0,img);
	}

public:

	QDataStream &toDataStream(QDataStream &ds)
	{
		OC_METHODGATE();
		ds <<base;
		return ds;
	}

	QDataStream &fromDataStream(QDataStream &ds)
	{
		OC_METHODGATE();
		ds >> base;
		return ds;
	}


};



static QDataStream &operator<<(QDataStream &ds, PixelTree &bb)
{
	OC_FUNCTIONGATE();
	return bb.toDataStream(ds);
}

static QDataStream &operator>>(QDataStream &ds, PixelTree &bb)
{
	OC_FUNCTIONGATE();
	return bb.fromDataStream(ds);
}


enum BlockType:quint8 {
	UNCOMPRESSED=0, AVGCOL, PIXEL_TREE
};

class BlockBlob
{
private:

	QImage img;
	QPoint loc;
	BlockType type;
public:

	BlockBlob()
		: img(px,px,fmt)
		, type(UNCOMPRESSED)
	{
		OC_METHODGATE();

	}

	BlockBlob(QImage &img, QPoint &loc, BlockType type=UNCOMPRESSED)
		: img(img)
		, loc(loc)
		, type(type)
	{
		OC_METHODGATE();

	}

	QDataStream &toDataStream(QDataStream &ds)
	{
		OC_METHODGATE();
		quint8 type_int=static_cast<quint8>(type);
		ds << type_int;
		ds << loc;
		switch(type) {
		case(UNCOMPRESSED): {
			ds << img;
		}
		break;
		case(AVGCOL): {
			QRgb ac = avgColor(img);
			ds << ac;
		}
		break;
		case(PIXEL_TREE): {
			PixelTree pt(px, img);
			ds << pt;
		}
		break;
			//default:			break;
		}
		return ds;
	}

	QDataStream &fromDataStream(QDataStream &ds)
	{
		OC_METHODGATE();
		quint8 type_int=0;
		ds >> type_int;
		type=static_cast<BlockType>(type_int);
		ds >> loc;

		switch(type) {

		case(UNCOMPRESSED): {
			ds >> img;
		}
		break;
		case(AVGCOL): {
			QRgb ac = 0;
			ds >> ac;
			QPainter p;
			p.begin(&img);
			p.setPen(Qt::NoPen);
			p.setBrush(QColor(ac));
			p.drawRect(0, 0, px, px);
		}
		break;
		case(PIXEL_TREE): {
			PixelTree pt(px);
			ds >> pt;
			QPainter p;
			p.begin(&img);
			p.setPen(Qt::NoPen);
			pt.draw(p);
		}
		break;
			//default:			break;
		}
		return ds;
	}


	void draw(QPainter &p)
	{
		OC_METHODGATE();
		p.drawImage(loc,img);
	}

};


static QDataStream &operator<<(QDataStream &ds, BlockBlob &bb)
{
	OC_FUNCTIONGATE();
	return bb.toDataStream(ds);
}

static QDataStream &operator>>(QDataStream &ds, BlockBlob &bb)
{
	OC_FUNCTIONGATE();
	return bb.fromDataStream(ds);
}



QByteArray StreamCodec::encode(QImage src)
{
	OC_METHODGATE();
	QByteArray out;
	QDataStream stream(&out, QIODevice::WriteOnly);
	if(fmt!=src.format()) {
		qDebug()<<"ENCODING FAILED WITH FORMAT MISMATCH; "<<src.format()<<" vs. "<<fmt;
		// Fail with 0x0 image
		stream<<QSize(0,0);
	} else {
		const QSize sz=src.size();
		QByteArray ba;
		{
			QDataStream ds(&ba, QIODevice::WriteOnly);
			ds<<src;
		}
		const auto basz=ba.size();
		stream<<sz;

		const QSize gsz((sz.width()+(px-1))/px, (sz.height()+(px-1))/px );
		const int w=gsz.width();
		const int h=gsz.height();

		for(int y=0; y<h; ++y) {

			for(int x=0; x<w; ++x) {
				QRect rect(x*px, y*px, px, px);
				//qDebug()<<"RECT: "<<rect;
				QImage block = src.copy(rect);
				//qDebug()<<"BLOCK: "<<block;
				QPoint loc=rect.topLeft();
				const quint8 type=qrand()%3;
				BlockBlob bb(block, loc, static_cast<BlockType>(type));
				stream << bb;
			}
		}
		qDebug().noquote().nospace()<< "ENCODING SIZES: pixels="
									<< sz
									<< ", png-bytes="
									<< utility::string::humanReadableSize(basz)
									<< ", bytes="<< utility::string::humanReadableSize(src.byteCount())
									<< ", compressed-bytes="
									<< utility::string::humanReadableSize(out.size());
	}
	return out;
}


QImage StreamCodec::decode(QByteArray src)
{
	OC_METHODGATE();
	QDataStream stream(&src, QIODevice::ReadOnly);
	QSize sz;
	stream>>sz;


	qDebug().noquote().nospace()<<"DECODING SIZES: pixels="<<sz<< ", compressed-bytes="<< utility::string::humanReadableSize(src.size());
	QImage out(sz.width(), sz.height(), fmt);

	const QSize gsz((sz.width()+(px-1))/px, (sz.height()+(px-1))/px );
	const int w=gsz.width();
	const int h=gsz.height();

	QPainter p;
	p.begin(&out);
	//p.setPen(QPen(Qt::red,1.0));
	p.setPen(Qt::NoPen);

	//.setBrush(Qt::black);
	p.fillRect(QRect(0,0,out.width(),out.height()), Qt::black);

	for(int y=0; y<h; ++y) {
		for(int x=0; x<w; ++x) {
			QRect rect(x*px, y*px, px, px);
			//QRgb ac=0;            stream>>ac;            p.setBrush(QColor(ac));            p.drawRect(rect);

			BlockBlob bb;
			stream >> bb;

			bb.draw(p);
		}
	}
	p.end();
	return out;
}
