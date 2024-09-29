#ifndef MARCHINGFIELDS_HPP
#define MARCHINGFIELDS_HPP

#include <QOpenGLWidget>

#include "audio/utils/AudioUtils.hpp"
#include "uptime/New.hpp"

#include <cmath> 

struct GLvector
{
	GLfloat fX;
	GLfloat fY;
	GLfloat fZ;
};



class Field{
protected:
	GLboolean bSpin{true};
	GLboolean bMove{true};
	
public:
	virtual void update(GLfloat fTime = 0.1f) = 0;
	virtual GLfloat density(const GLvector &pt) = 0;
	virtual GLfloat shine(const GLvector &pt) = 0;
	virtual void color(const GLvector &pt, GLvector &color) = 0;
	void setState(GLboolean spin, GLboolean move){
		bSpin = spin;
		bMove = move;
	}
};


class Blobs: public Field{
private:
	GLvector  sSourcePoint[3];
public:
	void update(GLfloat fTime = 0.1f) override;
	GLfloat density(const GLvector &pt) override;
	GLfloat shine(const GLvector &pt) override;
	void color(const GLvector &pt, GLvector &color) override;
};



class Bars: public Field{
private:
	GLvector  sSourcePoint[3];
public:
	void update(GLfloat fTime = 0.1f) override;
	GLfloat density(const GLvector &pt) override;
	GLfloat shine(const GLvector &pt) override;
	void color(const GLvector &pt, GLvector &color) override;
};



class Pond: public Field{
private:
	GLfloat   fTime{0.0f};
public:
	void update(GLfloat fTime = 0.1f) override;
	GLfloat density(const GLvector &pt) override;
	GLfloat shine(const GLvector &pt) override;
	void color(const GLvector &pt, GLvector &color) override;
};


class HeightMap{
private:
	int w{0};
	int h{0};
	GLfloat *data1{nullptr};
	GLfloat *data2{nullptr};
	GLfloat *back{nullptr};
	GLfloat *front{nullptr};

public:
	HeightMap(int w, int h)
		: w(w)
		, h(h)
		, data1(OC_NEW GLfloat[w*h])
		, data2(OC_NEW GLfloat[w*h])
	{
		swap();
		fill(0.0f);
		swap();
		fill(0.0f);
		//perturb(0.01);
	}
	inline void swap(){
		back = (back == data1)?data2:data1;
		front = (back == data1)?data2:data1;
		qDebug() << QString((back==data1)?"DATA1":"DATA2");
	}
	inline void fill(GLfloat v){
		int i{0};
		for(auto y=0;y<h;++y){
			for(auto x=0;x<w;++x){
				back[i++] = v;
			}
		}
	}
	inline void perturb(GLfloat v){
		int i{0};
		for(auto y=0;y<h;++y){
			for(auto x=0;x<w;++x){
				back[i++] = frand()*frand()*frand()*frand()*v;
			}
		}
	}
	inline void scale(GLfloat v){
		int i{0};
		for(auto y=0;y<h;++y){
			for(auto x=0;x<w;++x){
				back[i++] *= v;
			}
		}
	}
	inline void add(GLfloat v){
		int i{0};
		for(auto y=0;y<h;++y){
			for(auto x=0;x<w;++x){
				back[i++] += v;
			}
		}
	}
	inline void waveStep(){
		int i{0};
		int kernelSize{1};
		for(auto y=kernelSize;y<h-kernelSize;++y){
			for(auto x=kernelSize;x<w-kernelSize;++x){
				back[i] = (front[i-1] + front[i+1] + front[i-w] + front[i+w]) / 2 - back[i];
				i++;
			}
		}
		scale(0.999);
		//print();
		//back[(h/2)*w+w/2] = (frand()-0.5f)*0.05f;
	}
	inline GLfloat sampleTiling(GLfloat x, GLfloat y){
		x=std::fmod(x*w, w);
		y=std::fmod(y*h, h);
		int x0 = static_cast<int>(x);
		int y0 = static_cast<int>(y);
		if (x0 < 0) x0 = 0;
		if (y0 < 0) y0 = 0;
		if (x0 >= w-1) x0 = w - 2;
		if (y0 >= h-1) y0 = h - 2;
		int x1 = x0 + 1;
		int y1 = y0 + 1;
		GLfloat sx = x - static_cast<GLfloat>(x0);
		GLfloat sy = y - static_cast<GLfloat>(y0);
		GLfloat v00 = front[y0 * w + x0];
		GLfloat v10 = front[y0 * w + x1];
		GLfloat v01 = front[y1 * w + x0];
		GLfloat v11 = front[y1 * w + x1];
		GLfloat v0 = v00 + (v10 - v00) * sx; // Interpolate in x direction (top row)
		GLfloat v1 = v01 + (v11 - v01) * sx; // Interpolate in x direction (bottom row)
		GLfloat result = v0 + (v1 - v0) * sy; // Interpolate in y direction between the two rows
		return result;
	}

	inline GLfloat sample(GLfloat x, GLfloat y){
		int x0 = static_cast<int>(x*w);
		int y0 = static_cast<int>(y*h);
		if (x0 < 0) x0 = 0;
		if (y0 < 0) y0 = 0;
		if (x0 >= w-1) x0 = w - 2;
		if (y0 >= h-1) y0 = h - 2;
		int x1 = x0 + 1;
		int y1 = y0 + 1;
		GLfloat sx = x - static_cast<GLfloat>(x0);
		GLfloat sy = y - static_cast<GLfloat>(y0);
		GLfloat v00 = front[y0 * w + x0];
		GLfloat v10 = front[y0 * w + x1];
		GLfloat v01 = front[y1 * w + x0];
		GLfloat v11 = front[y1 * w + x1];
		GLfloat v0 = v00 + (v10 - v00) * sx; // Interpolate in x direction (top row)
		GLfloat v1 = v01 + (v11 - v01) * sx; // Interpolate in x direction (bottom row)
		GLfloat result = v0 + (v1 - v0) * sy; // Interpolate in y direction between the two rows
		return result;
	}

	inline void print(){
		int i{0};
		for(auto y=0;y<h;++y){
			QVector<GLfloat> v;
			for(auto x=0;x<w;++x){
				v<<back[i++];
			}
			qDebug()<<v;
		}
	}
};


class Birth: public Field{
private:
	HeightMap mMap;
	GLfloat   fTime{0.0f};
public:
	Birth()
		: mMap(14, 14)
	{
		
	}
public:
	void update(GLfloat fTime = 0.1f) override;
	GLfloat density(const GLvector &pt) override;
	GLfloat shine(const GLvector &pt) override;
	void color(const GLvector &pt, GLvector &color) override;
};



#endif // MARCHINGFIELDS_HPP
