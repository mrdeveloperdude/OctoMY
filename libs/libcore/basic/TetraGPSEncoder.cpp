#include "TetraGPSEncoder.hpp"

#include <QVector>
#include <QDebug>
#include <QtMath>

#include <limits>



Q_DECL_CONSTEXPR static inline bool qFuzzyIsNull(TGPSReal d) Q_REQUIRED_RESULT Q_DECL_UNUSED;
Q_DECL_CONSTEXPR static inline bool qFuzzyIsNull(TGPSReal d)
{
	return qAbs(d) <= std::numeric_limits<TGPSReal>::min();
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

		TGPSReal lat() const{
			TGPSReal xx=x();
			TGPSReal yy=y();
			return atan2(z(), sqrt(xx*xx+yy*yy))*180.0/M_PI;
		}

		TGPSReal lon() const{
			return atan2(y(), x())*180.0/M_PI;
		}

		QString latlon() const{
			return QString::number((double) lat(), 'f')+", "+QString::number((double) lon(), 'f');
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

Q_DECL_CONSTEXPR inline Vec::Vec() : xp(0.0f), yp(0.0f), zp(0.0f) {}


Q_DECL_CONSTEXPR inline TGPSReal Vec::x() const { return xp; }
Q_DECL_CONSTEXPR inline TGPSReal Vec::y() const { return yp; }
Q_DECL_CONSTEXPR inline TGPSReal Vec::z() const { return zp; }

inline void Vec::setX(TGPSReal aX) { xp = aX; }
inline void Vec::setY(TGPSReal aY) { yp = aY; }
inline void Vec::setZ(TGPSReal aZ) { zp = aZ; }

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
	return v1.xp == v2.xp && v1.yp == v2.yp && v1.zp == v2.zp;
}

Q_DECL_CONSTEXPR inline bool operator!=(const Vec &v1, const Vec &v2)
{
	return v1.xp != v2.xp || v1.yp != v2.yp || v1.zp != v2.zp;
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
	return qSqrt(xp * xp + yp * yp + zp * zp);
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
	if (qFuzzyIsNull(len - 1.0))
		return *this;
	else if (!qFuzzyIsNull(len))
		return *this / qSqrt(len);
	else
		return Vec();
}


void Vec::normalize()
{
	// Need some extra precision if the length is very small.
	TGPSReal len = TGPSReal(xp) * TGPSReal(xp) + TGPSReal(yp) * TGPSReal(yp) + TGPSReal(zp) * TGPSReal(zp);
	if (qFuzzyIsNull(len - 1.0) || qFuzzyIsNull(len))
		return;

	len = qSqrt(len);

	xp /= len;
	yp /= len;
	zp /= len;
}

struct Tri{
		Vec a;
		Vec b;
		Vec c;
		Vec center;
		int id;
		explicit Tri(Vec a=Vec(), Vec b=Vec(), Vec c=Vec(), int id=0)
			: a(a)
			, b(b)
			, c(c)
			, center(((a+b+c)/3.0).normalized())
			, id(id)
		{
		}

		QVector<Tri> subdivide(){
			QVector<Tri> faces;
			Vec d=((a+b)/2.0).normalized(), e=((b+c)/2.0).normalized(), f=((c+a)/2.0).normalized();
			faces.push_back(Tri(a,d,f,1));
			faces.push_back(Tri(d,b,e,2));
			faces.push_back(Tri(f,e,c,3));
			faces.push_back(Tri(d,e,f,4));
			return faces;
		}

		QString name(){
			return QString::number(id);
		}

		TGPSReal reach(){
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
	d.nospace() << QString::number((double) num, 'f');
	return d.maybeSpace();
}


inline QDebug &operator<<(QDebug &d, TGPSReal &num){
	d.nospace() << QString::number((double) num, 'f');
	return d.maybeSpace();
}



inline QDebug &operator<<(QDebug &dbg, const Vec &vector)
{
	dbg.nospace() << "Vec(" << vector.x() << ", " << vector.y() << ", " << vector.z() << ")";
	return dbg.space();
}


inline QDebug &operator<<(QDebug &d, Tri &tri){
	QString latlon= "LATLON:"+ tri.a.latlon();
	d.nospace() << "TRI< "<< tri.a <<latlon<< tri.b << tri.c << tri.center << " > reach:" << tri.reach();
	//d.nospace() <<latlon;
	return d.maybeSpace();
}



TetraGPSEncoder::TetraGPSEncoder(TGPSReal planetRadiusMeters):
	planetRadiusMeters(planetRadiusMeters)
{

}


QString TetraGPSEncoder::generate(TGPSReal lat, TGPSReal lon, TGPSReal precMeters){
	const Vec coord(cos(lat)*cos(lon), cos(lat)*sin(lon), sin(lat));
	qDebug()<<"COORD:"<<coord<<coord.length()<<coord.latlon();
	QVector<Tri> faces;
	const TGPSReal ma=(M_PI*2.0)/3.0;
	const Vec north(    +0.0,                                    +1.0,      +0.0);
	const Vec prime(    +0.0,                                    +sin(-ma), +cos(-ma));
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
	TGPSReal score=1000.0;
	QStringList ret;
	int sub=0;
	Tri best,second;
	while((score*planetRadiusMeters)>precMeters && sub++<9){
		//qDebug()<<"----ROUND "<<sub<< score;
		TGPSReal eps=1000.0;
		for(auto tri:faces) {
			TGPSReal delta=(tri.center-coord).length();
			//qDebug()<<"DELTA: "<<delta<<tri.center<<tri.center.length();
			if(delta<eps){
				if(delta<score){
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

		if(best!=second){
			second=best;
		}
		else{
			break;
		}
	}
	TGPSReal d=(coord-best.center).length()/best.reach();
	qDebug()<<"BEST: "<<best<<" with "<< d<<" normalized distance from center and " << (score*planetRadiusMeters)<<" meters after "<<sub<<" iterations";
	return ret.join(".");
}


