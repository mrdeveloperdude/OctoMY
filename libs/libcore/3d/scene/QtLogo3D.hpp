#ifndef QTLOGO3D_HPP
#define QTLOGO3D_HPP


#include <qopengl.h>
#include <QVector>
#include <QVector3D>

class QtLogo3D
{
public:
	QtLogo3D();
	const GLfloat *constData() const { return m_data.constData(); }
	int count() const { return m_count; }
	int vertexCount() const { return m_count / 6; }

private:
	void quad(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat x3, GLfloat y3, GLfloat x4, GLfloat y4);
	void extrude(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
	void add(const QVector3D &v, const QVector3D &n);

	QVector<GLfloat> m_data;
	int m_count;
};

#endif // QTLOGO3D_HPP
