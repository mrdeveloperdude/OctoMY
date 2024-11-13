#ifndef MARCHINGFIELDS_HPP
#define MARCHINGFIELDS_HPP

#include <QOpenGLWidget>

#include <cmath>

#include "HeightMap.hpp"
#include "HeightWidget.hpp"
#include "GLvector.hpp"


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

struct Shape{
	GLvector position;
	GLfloat magnitude{0.001f};
	GLfloat radius{0.025f};
	void update(){
		position.fZ += 0.025f;
		if(position.fZ > 1.0f){
			position.fZ -= 2.0f;
		}
	}
	GLfloat density(const GLvector &pt) const;
};


class Birth: public Field{
private:
	HeightMap mMap;
	HeightWidget *mWidget;
	GLfloat   fTime{0.0f};
	GLfloat ma{-9999999.0f}, mi{-ma};
	QVector <Shape> mShapes;

public:
	Birth();

public:
	void update(GLfloat fTime = 0.1f) override;
	GLfloat density(const GLvector &pt) override;
	GLfloat shine(const GLvector &pt) override;
	void color(const GLvector &pt, GLvector &color) override;

};



#endif // MARCHINGFIELDS_HPP
