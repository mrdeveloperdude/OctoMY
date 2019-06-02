#include "TetraGPSEncoder.hpp"

#include <QVector>
#include <QDebug>
#include <QtMath>

#include <limits>

#include <QtMath>


Q_REQUIRED_RESULT Q_DECL_CONSTEXPR static inline Q_DECL_UNUSED bool fuzzy_is_null(TGPSReal d)
{
	return std::abs(d) <= std::numeric_limits<TGPSReal>::min();
}


Q_REQUIRED_RESULT Q_DECL_CONSTEXPR static inline Q_DECL_UNUSED TGPSReal sqrt(TGPSReal d)
{
	return std::sqrt(d);
}


Q_REQUIRED_RESULT Q_DECL_CONSTEXPR static inline Q_DECL_UNUSED TGPSReal sin(TGPSReal d)
{
	return std::sin(d);
}


Q_REQUIRED_RESULT Q_DECL_CONSTEXPR static inline Q_DECL_UNUSED TGPSReal cos(TGPSReal d)
{
	return std::cos(d);
}


Q_REQUIRED_RESULT Q_DECL_CONSTEXPR static inline Q_DECL_UNUSED TGPSReal tan(TGPSReal d)
{
	return std::tan(d);
}


Q_REQUIRED_RESULT Q_DECL_CONSTEXPR static inline Q_DECL_UNUSED TGPSReal min(TGPSReal a, TGPSReal b)
{
	return std::min(a, b);
}


Q_REQUIRED_RESULT Q_DECL_CONSTEXPR static inline Q_DECL_UNUSED TGPSReal max(TGPSReal a, TGPSReal b)
{
	return std::max(a, b);
}


Q_REQUIRED_RESULT Q_DECL_CONSTEXPR static inline Q_DECL_UNUSED TGPSReal atan2(TGPSReal a, TGPSReal b)
{
	return std::atan2(a, b);
}


Q_REQUIRED_RESULT Q_DECL_CONSTEXPR static inline Q_DECL_UNUSED bool fuzzy_compare(TGPSReal a, TGPSReal b)
{
	return (std::abs(a - b) * 1000000000000.L <= std::min(std::abs(a), std::abs(b)));
}


class Vec
{
private:
	TGPSReal xp, yp, zp;

public:
	Q_DECL_CONSTEXPR Vec();
	Q_DECL_CONSTEXPR Vec(TGPSReal xpos, TGPSReal ypos, TGPSReal zpos) : xp(xpos), yp(ypos), zp(zpos) {}

	Q_DECL_CONSTEXPR TGPSReal x() const;
	Q_DECL_CONSTEXPR TGPSReal y() const;
	Q_DECL_CONSTEXPR TGPSReal z() const;

	void setX(TGPSReal x);
	void setY(TGPSReal y);
	void setZ(TGPSReal z);

	TGPSReal &operator[](int i);
	TGPSReal operator[](int i) const;

	TGPSReal length() const;
	TGPSReal lengthSquared() const;
	Vec normalized() const;
	void normalize();
	Vec &operator+=(const Vec &vector);
	Vec &operator-=(const Vec &vector);
	Vec &operator*=(TGPSReal factor);
	Vec &operator*=(const Vec& vector);
	Vec &operator/=(TGPSReal divisor);

	TGPSReal lat() const
	{
		TGPSReal xx=x();
		TGPSReal yy=y();
		return atan2(z(), sqrt(xx*xx+yy*yy))*180.0L / static_cast<long double>(M_PI);
	}


	TGPSReal lon() const
	{
		return atan2(y(), x())*180.0L / static_cast<long double>(M_PI);
	}


	QString latlon() const
	{
		return QString::number(static_cast<double>(lat()), 'f')+", "+QString::number(static_cast<double>(lon()), 'f');
	}


	inline Vec &operator/=(const Vec &vector);
	static Vec normal(const Vec& v1, const Vec& v2);
	static Vec normal(const Vec& v1, const Vec& v2, const Vec& v3);
	Q_DECL_CONSTEXPR friend inline bool operator==(const Vec &v1, const Vec &v2);
	Q_DECL_CONSTEXPR friend inline bool operator!=(const Vec &v1, const Vec &v2);
	Q_DECL_CONSTEXPR friend inline const Vec operator+(const Vec &v1, const Vec &v2);
	Q_DECL_CONSTEXPR friend inline const Vec operator-(const Vec &v1, const Vec &v2);
	Q_DECL_CONSTEXPR friend inline const Vec operator*(TGPSReal factor, const Vec &vector);
	Q_DECL_CONSTEXPR friend inline const Vec operator*(const Vec &vector, TGPSReal factor);
	Q_DECL_CONSTEXPR friend const Vec operator*(const Vec &v1, const Vec& v2);
	Q_DECL_CONSTEXPR friend inline const Vec operator-(const Vec &vector);
	Q_DECL_CONSTEXPR friend inline const Vec operator/(const Vec &vector, TGPSReal divisor);
	Q_DECL_CONSTEXPR friend inline const Vec operator/(const Vec &vector, const Vec &divisor);

	//	friend inline const QDebug &operator<<(const QDebug &d, const Vec &vec);
};


Q_DECL_CONSTEXPR inline Vec::Vec() : xp(0.0l), yp(0.0l), zp(0.0l) {}


Q_DECL_CONSTEXPR inline TGPSReal Vec::x() const
{
	return xp;
}


Q_DECL_CONSTEXPR inline TGPSReal Vec::y() const
{
	return yp;
}


Q_DECL_CONSTEXPR inline TGPSReal Vec::z() const
{
	return zp;
}


inline void Vec::setX(TGPSReal aX)
{
	xp = aX;
}


inline void Vec::setY(TGPSReal aY)
{
	yp = aY;
}


inline void Vec::setZ(TGPSReal aZ)
{
	zp = aZ;
}


inline TGPSReal &Vec::operator[](int i)
{
	Q_ASSERT(uint(i) < 3u);
	return *(&xp + i);
}


inline TGPSReal Vec::operator[](int i) const
{
	Q_ASSERT(uint(i) < 3u);
	return *(&xp + i);
}


inline Vec &Vec::operator+=(const Vec &vector)
{
	xp += vector.xp;
	yp += vector.yp;
	zp += vector.zp;
	return *this;
}


inline Vec &Vec::operator-=(const Vec &vector)
{
	xp -= vector.xp;
	yp -= vector.yp;
	zp -= vector.zp;
	return *this;
}


inline Vec &Vec::operator*=(TGPSReal factor)
{
	xp *= factor;
	yp *= factor;
	zp *= factor;
	return *this;
}


inline Vec &Vec::operator*=(const Vec& vector)
{
	xp *= vector.xp;
	yp *= vector.yp;
	zp *= vector.zp;
	return *this;
}


inline Vec &Vec::operator/=(TGPSReal divisor)
{
	xp /= divisor;
	yp /= divisor;
	zp /= divisor;
	return *this;
}


inline Vec &Vec::operator/=(const Vec &vector)
{
	xp /= vector.xp;
	yp /= vector.yp;
	zp /= vector.zp;
	return *this;
}


Q_DECL_CONSTEXPR inline bool operator==(const Vec &v1, const Vec &v2)
{
	return fuzzy_compare(v1.xp, v2.xp) && fuzzy_compare(v1.yp, v2.yp) && fuzzy_compare(v1.zp, v2.zp);
}


Q_DECL_CONSTEXPR inline bool operator!=(const Vec &v1, const Vec &v2)
{
	return !fuzzy_compare(v1.xp, v2.xp) || !fuzzy_compare(v1.yp, v2.yp) || fuzzy_compare(v1.zp, v2.zp);
}


Q_DECL_CONSTEXPR inline const Vec operator+(const Vec &v1, const Vec &v2)
{
	return Vec(v1.xp + v2.xp, v1.yp + v2.yp, v1.zp + v2.zp);
}


Q_DECL_CONSTEXPR inline const Vec operator-(const Vec &v1, const Vec &v2)
{
	return Vec(v1.xp - v2.xp, v1.yp - v2.yp, v1.zp - v2.zp);
}


Q_DECL_CONSTEXPR inline const Vec operator*(TGPSReal factor, const Vec &vector)
{
	return Vec(vector.xp * factor, vector.yp * factor, vector.zp * factor);
}


Q_DECL_CONSTEXPR inline const Vec operator*(const Vec &vector, TGPSReal factor)
{
	return Vec(vector.xp * factor, vector.yp * factor, vector.zp * factor);
}


Q_DECL_CONSTEXPR inline const Vec operator*(const Vec &v1, const Vec& v2)
{
	return Vec(v1.xp * v2.xp, v1.yp * v2.yp, v1.zp * v2.zp);
}


Q_DECL_CONSTEXPR inline const Vec operator-(const Vec &vector)
{
	return Vec(-vector.xp, -vector.yp, -vector.zp);
}


Q_DECL_CONSTEXPR inline const Vec operator/(const Vec &vector, TGPSReal divisor)
{
	return Vec(vector.xp / divisor, vector.yp / divisor, vector.zp / divisor);
}


Q_DECL_CONSTEXPR inline const Vec operator/(const Vec &vector, const Vec &divisor)
{
	return Vec(vector.xp / divisor.xp, vector.yp / divisor.yp, vector.zp / divisor.zp);
}


TGPSReal Vec::length() const
{
	return std::sqrt(static_cast<TGPSReal>(xp * xp + yp * yp + zp * zp));
}


TGPSReal Vec::lengthSquared() const
{
	return xp * xp + yp * yp + zp * zp;
}


Vec Vec::normalized() const
{
	// Need some extra precision if the length is very small.
	TGPSReal len = TGPSReal(xp) * TGPSReal(xp) +
				   TGPSReal(yp) * TGPSReal(yp) +
				   TGPSReal(zp) * TGPSReal(zp);
	if (fuzzy_is_null(len - 1.0L)) {
		return *this;
	} else if (!fuzzy_is_null(len)) {
		return *this / sqrt(len);
	} else {
		return Vec();
	}
}


void Vec::normalize()
{
	// Need some extra precision if the length is very small.
	TGPSReal len = TGPSReal(xp) * TGPSReal(xp) + TGPSReal(yp) * TGPSReal(yp) + TGPSReal(zp) * TGPSReal(zp);
	if (fuzzy_is_null(len - 1.0L) || fuzzy_is_null(len)) {
		return;
	}

	len = sqrt(len);

	xp /= len;
	yp /= len;
	zp /= len;
}


struct Tri {
	Vec a;
	Vec b;
	Vec c;
	Vec center;
	int id;
	explicit Tri(Vec a=Vec(), Vec b=Vec(), Vec c=Vec(), int id=0)
		: a(a)
		, b(b)
		, c(c)
		, center(((a+b+c)/3.0L).normalized())
		, id(id)
	{
	}


	QVector<Tri> subdivide()
	{
		QVector<Tri> faces;
		Vec d=((a+b)/2.0L).normalized(), e=((b+c)/2.0L).normalized(), f=((c+a)/2.0L).normalized();
		faces.push_back(Tri(a,d,f,1));
		faces.push_back(Tri(d,b,e,2));
		faces.push_back(Tri(f,e,c,3));
		faces.push_back(Tri(d,e,f,4));
		return faces;
	}


	QString name()
	{
		return QString::number(id);
	}


	TGPSReal reach()
	{
		TGPSReal d=(a-center).lengthSquared();
		TGPSReal e=(b-center).lengthSquared();
		TGPSReal f=(c-center).lengthSquared();
		return sqrt(qMax(qMax(d,e),f));
	}

	Q_DECL_CONSTEXPR friend inline bool operator==(const Tri &v1, const Tri &v2);
	Q_DECL_CONSTEXPR friend inline bool operator!=(const Tri &v1, const Tri &v2);


	//	friend inline QDebug &operator<<(QDebug &d, Tri &tri);
};


Q_DECL_CONSTEXPR inline bool operator==(const Tri &v1, const Tri &v2)
{
	return v1.a == v2.a && v1.b == v2.b && v1.c == v2.c;
}


Q_DECL_CONSTEXPR inline bool operator!=(const Tri &v1, const Tri &v2)
{
	return v1.a != v2.a || v1.b != v2.b || v1.c != v2.c;
}


inline QDebug operator<<(QDebug d, const TGPSReal &num)
{
	d.nospace() << QString::number(static_cast<double>(num), 'f');
	return d.maybeSpace();
}


inline QDebug &operator<<(QDebug &d, TGPSReal &num)
{
	d.nospace() << QString::number(static_cast<double>(num), 'f');
	return d.maybeSpace();
}


inline QDebug &operator<<(QDebug &dbg, const Vec &vector)
{
	dbg.nospace() << "Vec(" << vector.x() << ", " << vector.y() << ", " << vector.z() << ")";
	return dbg.space();
}


inline QDebug &operator<<(QDebug &d, Tri &tri)
{
	QString latlon= "LATLON:"+ tri.a.latlon();
	d.nospace() << "TRI< "<< tri.a <<latlon<< tri.b << tri.c << tri.center << " > reach:" << tri.reach();
	//d.nospace() <<latlon;
	return d.maybeSpace();
}


TetraGPSEncoder::TetraGPSEncoder(TGPSReal planetRadiusMeters):
	planetRadiusMeters(planetRadiusMeters)
{

}


QString TetraGPSEncoder::generate(TGPSReal lat, TGPSReal lon, TGPSReal precMeters)
{
	const Vec coord(cos(lat)*cos(lon), cos(lat)*sin(lon), sin(lat));
	qDebug()<<"COORD:"<<coord<<coord.length()<<coord.latlon();
	QVector<Tri> faces;
	const TGPSReal ma=(static_cast<TGPSReal>(M_PI)*2.0L)/3.0L;
	const Vec north(    +0.0L,                                    +1.0L,      +0.0L);
	const Vec prime(    +0.0L,                                    +sin(-ma), +cos(-ma));
	const Vec meridian2(prime.x()*cos(+ma) - prime.z()*sin(+ma), prime.y(), prime.z()*cos(+ma) + prime.x()*sin(+ma));
	const Vec meridian3(prime.x()*cos(-ma) - prime.z()*sin(-ma), prime.y(), prime.z()*cos(-ma) + prime.x()*sin(-ma));
	qDebug()<<"north=    "<<north<<north.length();
	qDebug()<<"prime=    "<<prime<<prime.length();
	qDebug()<<"meridian2="<<meridian2<<meridian2.length();
	qDebug()<<"meridian3="<<meridian3<<meridian3.length();
	faces.push_back(Tri(north,prime,meridian2, 1));
	faces.push_back(Tri(north,prime,meridian3, 2));
	faces.push_back(Tri(north,meridian2,meridian3, 3));
	faces.push_back(Tri(prime,meridian2,meridian3, 4));
	TGPSReal score=1000.0L;
	QStringList ret;
	int sub=0;
	Tri best,second;
	while((score*planetRadiusMeters)>precMeters && sub++<9) {
		//qDebug()<<"----ROUND "<<sub<< score;
		TGPSReal eps=1000.0L;
		for(auto tri:faces) {
			TGPSReal delta=(tri.center-coord).length();
			//qDebug()<<"DELTA: "<<delta<<tri.center<<tri.center.length();
			if(delta<eps) {
				if(delta<score) {
					score=delta;
				}
				eps=delta;
				best=tri;
				qDebug()<<"NEW BEST: "<<best<<"!";
			}
		}
		QString name=best.name();
		//if("0"==name){			break;		}
		ret<<name;
		faces=best.subdivide();

		if(best!=second) {
			second=best;
		} else {
			break;
		}
	}
	TGPSReal d=(coord-best.center).length()/best.reach();
	qDebug()<<"BEST: "<<best<<" with "<< d<<" normalized distance from center and " << (score*planetRadiusMeters)<<" meters after "<<sub<<" iterations";
	return ret.join(".");
}
