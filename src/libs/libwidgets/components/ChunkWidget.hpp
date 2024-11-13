#ifndef CHUNKWIDGET_HPP
#define CHUNKWIDGET_HPP

#ifndef ANDROID

#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_4_3_Core>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>
#include <QTimer>
#include <qelapsedtimer.h>

#include "GLvector.hpp"

class QOpenGLShaderProgram;



class Chunk{
private:
	QOpenGLFunctions_4_3_Core *gl{nullptr};
	int num_voxels_per_side{0};
	int num_voxels{0};
	GLfloat voxel_size{0.0f};
	GLvector pos{0.0f, 0.0f, 0.0f};
	QElapsedTimer  mTime;
	
	
public:
	Chunk(QOpenGLContext *context, int voxels_per_side=8, GLfloat voxel_size=0.15f, GLvector pos = GLvector(0,0,0) );
	
	void create_input_buffer();
	void create_geometry_buffer();
	void create_geometry_node();
	void create_compute_node();
	
	
	void update(){
		
	}
};



class ChunkWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_3_Core
{
	Q_OBJECT
private:
	QOpenGLShaderProgram *program = nullptr;
	QOpenGLShaderProgram *computeProgram = nullptr;
	QOpenGLBuffer voxelBuffer;  // To store voxel data
	QOpenGLVertexArrayObject vao;
	QStringList mProcessed;
	QElapsedTimer  mTime;
	bool mFilp{false};
	int mAxis{5};

public:
	explicit ChunkWidget(QWidget *parent = nullptr);

	// QOpenGLWidget infterface
protected:
	void initializeGL() override;
	void resizeGL(int w, int h) override;
	void paintGL() override;

private:
	QString preprocessShader(const QString &filePath);
	void initShaders();
	void initShadersFast();
	void initBuffers();
	void runComputeShader();
	void renderVoxels();

};


#endif // ANDROID

#endif // CHUNKWIDGET_HPP
