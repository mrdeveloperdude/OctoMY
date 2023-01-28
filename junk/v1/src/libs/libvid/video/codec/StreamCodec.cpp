#include "StreamCodec.hpp"

#include "utility/Utility.hpp"

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

}

static quint32 avgColor(const QImage &img)
{
	quint32 r=0,g=0,b=0, a=0;
	const unsigned int w=img.width();
	const unsigned int h=img.height();
	const unsigned int m=w*h;
	for(unsigned  int y=0; y<h; ++y) {
		for(unsigned int x=0; x<w; ++x) {
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
	quint32 r=0,g=0,b=0, a=0;
	const unsigned int w=rect.width();
	const unsigned int h=rect.height();
	const unsigned int m=w*h;
	const unsigned int x1=rect.x();
	const unsigned int y1=rect.y();
	for(unsigned  int y=0; y<h; ++y) {
		for(unsigned int x=0; x<w; ++x) {
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

	}

	void set(PixelData &a1, PixelData &a2, PixelData &b1, PixelData &b2)
	{
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
		ds << avg;
		ds << min;
		ds << max;
		return ds;
	}

	QDataStream &fromDataStream(QDataStream &ds)
	{
		ds >> avg;
		ds >> min;
		ds >> max;
		return ds;
	}

};





static QDataStream &operator<<(QDataStream &ds, PixelData &bb)
{
	return bb.toDataStream(ds);
}

static QDataStream &operator>>(QDataStream &ds, PixelData &bb)
{
	return bb.fromDataStream(ds);
}


struct PixelNode;

static QDataStream &operator<<(QDataStream &ds, PixelNode &bb);
static QDataStream &operator>>(QDataStream &ds, PixelNode &bb);

quint32 rgb2lum(QRgb rgb)
{
	const quint32 r=qRed(rgb)*29;
	const quint32 g=qRed(rgb)*11;
	const quint32 b=qRed(rgb)*59;
	const quint32 ret= (r+g+b)/100;
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

	}

	void free()
	{
#define PIXEL_NODE_FREE(a) if(nullptr!=a){ a->free(); delete a; a=nullptr; }
		PIXEL_NODE_FREE(a1);
		PIXEL_NODE_FREE(a2);
		PIXEL_NODE_FREE(b1);
		PIXEL_NODE_FREE(b2);
#undef PIXEL_NODE_FREE
	}

	void set(PixelNode &a1, PixelNode &a2, PixelNode &b1, PixelNode &b2)
	{
		this->a1=&a1;
		this->a2=&a2;
		this->b1=&b1;
		this->b2=&b2;

		data.set(a1.data, a2.data, b1.data, b2.data);
	}


public:

	QDataStream &toDataStream(QDataStream &ds)
	{
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
	return bb.toDataStream(ds);
}

static QDataStream &operator>>(QDataStream &ds, PixelNode &bb)
{
	return bb.fromDataStream(ds);
}






class PixelTree
{
private:
	const quint32 px;
	QImage img;
	QVector<PixelData> nodes;

	PixelNode base;

	QPainter p;
public:
	PixelTree(quint32 px, QImage img)
		: px(px)
		, img(img)
		, nodes(px*px)
	{

	}

	PixelTree(quint32 px)
		: px(px)
		, nodes(px*px)
	{

	}

	void generate()
	{
		base.free();
		generateWorker(base, 0,0,0);
	}

private:

	PixelNode &generateWorker(PixelNode &node, const quint32 x, const quint32 y, quint32 level)
	{
		if(px==level) {
			PixelData me(img.pixel(x,y));
			node.data=me;
			return node;
		} else {
			++level;
			const quint32 nx=x|(1<<level);
			const quint32 ny=y|(1<<level);
			PixelNode &a1=generateWorker(*(OC_NEW PixelNode(node)), x ,y , level);
			PixelNode &a2=generateWorker(*(OC_NEW PixelNode(node)), nx,y , level);
			PixelNode &b1=generateWorker(*(OC_NEW PixelNode(node)), x ,ny, level);
			PixelNode &b2=generateWorker(*(OC_NEW PixelNode(node)), nx,ny, level);
			node.set(a1, a2, b1, b2);
			return node;
		}
	}

	void reconstructWorker(PixelNode &node, const quint32 x, const quint32 y, quint32 level)
	{
		const quint32 area=(1<<(px-level) );
		p.setBrush(QColor(node.data.avg));
		p.drawRect(QRect(x,y,area,area));
		++level;
		const quint32 nx=x|(1<<level);
		const quint32 ny=y|(1<<level);
		if(nullptr!=node.a1) {
			reconstructWorker(*node.a1, x ,y ,level);
		}
		if(nullptr!=node.a2) {
			reconstructWorker(*node.a2, nx ,y ,level);
		}
		if(nullptr!=node.b1) {
			reconstructWorker(*node.b1, x ,ny ,level);
		}
		if(nullptr!=node.b2) {
			reconstructWorker(*node.b2, nx ,ny ,level);
		}

	}

public:

	void reconstruct()
	{
		img=QImage(px,px,fmt);
		p.begin(&img);
		p.setPen(Qt::NoPen);
		reconstructWorker(base, 0, 0, 0);
		p.end();
	}

	void draw(QPainter &p)
	{
		reconstruct();
		p.drawImage(0,0,img);
	}

public:

	QDataStream &toDataStream(QDataStream &ds)
	{
		ds <<base;
		return ds;
	}

	QDataStream &fromDataStream(QDataStream &ds)
	{
		ds >> base;
		return ds;
	}


};



static QDataStream &operator<<(QDataStream &ds, PixelTree &bb)
{
	return bb.toDataStream(ds);
}

static QDataStream &operator>>(QDataStream &ds, PixelTree &bb)
{
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

	}

	BlockBlob(QImage &img, QPoint &loc, BlockType type=UNCOMPRESSED)
		: img(img)
		, loc(loc)
		, type(type)
	{

	}

	QDataStream &toDataStream(QDataStream &ds)
	{
		quint8 type_int=(quint8)type;
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
		default:
			break;
		}
		return ds;
	}

	QDataStream &fromDataStream(QDataStream &ds)
	{
		quint8 type_int=0;
		ds >> type_int;
		type=(BlockType)type_int;
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
		default:
			break;
		}
		return ds;
	}


	void draw(QPainter &p)
	{
		p.drawImage(loc,img);
	}

};


static QDataStream &operator<<(QDataStream &ds, BlockBlob &bb)
{
	return bb.toDataStream(ds);
}

static QDataStream &operator>>(QDataStream &ds, BlockBlob &bb)
{
	return bb.fromDataStream(ds);
}



QByteArray StreamCodec::encode(QImage src)
{
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
				BlockBlob bb(block, loc, (BlockType)type);
				stream << bb;
			}
		}
		qDebug().noquote().nospace()<<"ENCODING SIZES: pixels="<<sz<<", png-bytes="<< utility::humanReadableSize(basz)<< ", bytes="<< utility::humanReadableSize(src.byteCount()) << ", compressed-bytes="<< utility::humanReadableSize(out.size());
	}
	return out;
}


QImage StreamCodec::decode(QByteArray src)
{
	QDataStream stream(&src, QIODevice::ReadOnly);
	QSize sz;
	stream>>sz;


	qDebug().noquote().nospace()<<"DECODING SIZES: pixels="<<sz<< ", compressed-bytes="<< utility::humanReadableSize(src.size());
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
