#ifndef ANDROID
#include "ChunkWidget.hpp"


#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QOpenGLShaderProgram>
#include <QRegularExpression>
#include <QTextStream>
#include <QOpenGLVersionFunctionsFactory>
#include <QOpenGLFunctions_4_3_Core>


Chunk::Chunk(QOpenGLContext *context, int voxels_per_side, GLfloat voxel_size, GLvector pos ){
	gl = QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_4_3_Core>(context);
	if (!gl) {
		qFatal("Could not obtain required OpenGL context version");
	}
	num_voxels_per_side = voxels_per_side + 1; // padding
	num_voxels = (num_voxels_per_side*num_voxels_per_side*num_voxels_per_side);
	this->voxel_size = voxel_size;
	this->pos = pos;
	auto scale = 0.6;
	create_input_buffer();
	create_geometry_buffer();
	create_geometry_node();
	create_compute_node();
}

float some_noise_function(GLfloat x, GLfloat y, GLfloat z){
	static const GLfloat A{10.0};
	return sin(x * A) * sin(y * A) * sin(z * A);
	
}

void Chunk::create_input_buffer() {
	/*
	// Create a buffer to hold voxel data
	QOpenGLBuffer inputBuffer(QOpenGLBuffer::ShaderStorageBuffer);
	inputBuffer.create();
	inputBuffer.bind();
	
	// Create a voxel field initialized with noise or some values
	std::vector<float> field(num_voxels, -1.0f); // Example initialization
	
	// Fill the field with some values, perhaps noise-based, as in your Python code
	int i = 0;
	for (int z = 0; z < num_voxels_per_side; z++) {
		for (int y = 0; y < num_voxels_per_side; y++) {
			for (int x = 0; x < num_voxels_per_side; x++) {
				float px = pos.fX + x * voxel_size;
				float py = pos.fY + y * voxel_size;
				float pz = pos.fZ + z * voxel_size;
				// Apply some transformation to py as in your Python code
				py += mTime.elapsed() * 0.002; // Adjust scale factor as necessary
				field[i] = some_noise_function(px, py, pz); // Use your noise function
				i++;
			}
		}
	}
	
	// Allocate and fill the buffer
	inputBuffer.allocate(field.data(), field.size() * sizeof(float));
	inputBuffer.release();
*/
}


void Chunk::create_geometry_buffer() {
	/*
	// Create the geometry buffer (SSBO) for voxel geometry
	QOpenGLBuffer geomBuffer(QOpenGLBuffer::ShaderStorageBuffer);
	geomBuffer.create();
	geomBuffer.bind();
	
	// Maximum number of vertices per voxel (15 vertices per voxel as per your Python code)
	const int max_points_per_voxel = 15;
	
	// Initialize the vertex buffer with some dummy data
	std::vector<float> vertices(num_voxels * max_points_per_voxel * 8, 0.0f);  // 8 floats per vertex (x, y, z, normal, color)
	
	// Allocate and fill the buffer
	geomBuffer.allocate(vertices.data(), vertices.size() * sizeof(float));
	geomBuffer.release();
*/
}


void Chunk::create_geometry_node() {
	/*
	// Create the VAO for rendering
	QOpenGLVertexArrayObject vao;
	vao.create();
	vao.bind();
	
	// Bind the geometry buffer (this assumes that we will render the geometry from this buffer)
	QOpenGLBuffer geomBuffer(QOpenGLBuffer::ShaderStorageBuffer);
	geomBuffer.bind();
	
	// Configure the VAO (set up vertex attributes like position, normal, color, etc.)
	// Example assuming 8 floats per vertex: x, y, z, w, r, g, b, a
	gl->glEnableVertexAttribArray(0); // Position
	gl->glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	gl->glEnableVertexAttribArray(1); // Color
	gl->glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(4 * sizeof(float)));
	
	// Unbind the VAO and buffer
	vao.release();
	geomBuffer.release();
*/
}


void Chunk::create_compute_node() {
	/*
	// Assuming you have a `QOpenGLShaderProgram` for the compute shader
	QOpenGLShaderProgram computeProgram;
	computeProgram.create();
	computeProgram.addShaderFromSourceFile(QOpenGLShader::Compute, ":/shaders/geometry.comp");
	computeProgram.link();
	computeProgram.bind();
	
	// Bind the input and geometry buffers as Shader Storage Buffer Objects (SSBOs)
	inputBuffer.bindBase(QOpenGLBuffer::ShaderStorageBuffer, 0);  // Binding point 0 for input data
	geomBuffer.bindBase(QOpenGLBuffer::ShaderStorageBuffer, 1);  // Binding point 1 for geometry data
	
	// Set shader uniforms
	computeProgram.setUniformValue("voxel_size", voxel_size);
	computeProgram.setUniformValue("threshold", 0.2f);  // Example threshold
	
	// Dispatch the compute shader (assuming chunk size 9x9x9 as in the original)
	glDispatchCompute(num_voxels_per_side / 9, num_voxels_per_side / 9, num_voxels_per_side / 9);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);  // Ensure SSBOs are updated
	
	computeProgram.release();
*/
}



// Function to recursively load shader source and resolve #pragma include and #pragma once directives
QString ChunkWidget::preprocessShader(const QString &filePath) {
	auto out = qDebug().noquote().nospace();
	const auto alreadySeen = mProcessed.contains(filePath);
	mProcessed << filePath;
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qWarning() << "Unable to open shader file:" << filePath;
		return QString();
	}
	QTextStream in(&file);
	auto shaderSource = in.readAll();
	file.close();

	QRegularExpression onceRegex(R"REGEX(#pragma\s+once\s*")REGEX");
	auto onceMatches = onceRegex.globalMatch(shaderSource);
	while(onceMatches.hasNext()){
		// Put pragma once into effect
		if(alreadySeen){
			out<<"\n";
			out<<"SHADER ----------------------\n";
			out<<"SHADER" << filePath << ": PRAGMA ONCE SKIPPED\n";
			out<<"\n";
			return QString();
		}
		auto match = onceMatches.next();
		shaderSource.replace(match.capturedStart(), match.capturedLength(), "");
	}
	
	QRegularExpression includeRegex(R"REGEX(#pragma\s+include\s+"([^"]+)")REGEX");
	auto matches = includeRegex.globalMatch(shaderSource);
	while (matches.hasNext()) {
		auto match = matches.next();
		QString includeFile = match.captured(1);
		QString includePath = QFileInfo(filePath).absolutePath() + "/" + includeFile;
		QString includeContent = preprocessShader(includePath);
		shaderSource.replace(match.capturedStart(), match.capturedLength(), includeContent);
	}
	out<<"\n";
	out<<"SHADER ----------------------\n";
	out<<"SHADER" << filePath << ":\n";
	//out<<"SHADER" << shaderSource;
	out<<"\n";
	return shaderSource;
}




ChunkWidget::ChunkWidget(QWidget *parent)
	: QOpenGLWidget(parent) {}


void ChunkWidget::initializeGL() {
	initializeOpenGLFunctions();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	GLenum    ePolygonMode{GL_FILL};
	glPolygonMode(GL_FRONT_AND_BACK, ePolygonMode);
	GLfloat afPropertiesAmbient[] = {0.50, 0.50, 0.50, 1.00};
	GLfloat afPropertiesDiffuse[] = {0.75, 0.75, 0.75, 1.00};
	GLfloat afPropertiesSpecular[] = {1.00, 1.00, 1.00, 1.00};
	glLightfv(GL_LIGHT0, GL_AMBIENT, afPropertiesAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, afPropertiesDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, afPropertiesSpecular);
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 1.0);
	glEnable(GL_LIGHT0);
	//initShaders();
	initShadersFast();
	initBuffers();
}

void ChunkWidget::resizeGL(int w, int h) {
	const GLfloat fWorldSize = 1.f;
	const GLfloat fWorldFar = 2.f;
	const GLfloat fHalfWorldSize = 0.5f * fWorldSize;
	GLfloat fAspect;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w > h){
		fAspect = (GLfloat)h / (GLfloat)w;
		glOrtho(-fHalfWorldSize, fHalfWorldSize, -fHalfWorldSize*fAspect, fHalfWorldSize*fAspect, -fWorldFar*fHalfWorldSize, fWorldFar*fHalfWorldSize);
	}
	else{
		fAspect = (GLfloat)w / (GLfloat)h; 
		glOrtho(-fHalfWorldSize*fAspect, fHalfWorldSize*fAspect, -fHalfWorldSize, fHalfWorldSize, -fWorldFar*fHalfWorldSize, fWorldFar*fHalfWorldSize);
	}
	
	glMatrixMode(GL_MODELVIEW);
}

void ChunkWidget::paintGL() {
	mFilp=!mFilp;
	glClearColor(mFilp?0.8666f:0.098f, 0.098, mFilp?0.098f:0.8666f, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	glTranslatef(0.5f, -2.0f, 0.5f);
	runComputeShader();
	renderVoxels();
	glPopMatrix();
	update();
}


void ChunkWidget::initShaders() {
	const auto base = QString(":/3d/shaders/isosurface/");
	program = new QOpenGLShaderProgram(this);
	QString vertexShaderSource = preprocessShader(base + "geometry.vert");
	QString fragmentShaderSource = preprocessShader(base + "geometry.frag");
	program->addCacheableShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
	program->addCacheableShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
	program->link();
	computeProgram = new QOpenGLShaderProgram(this);
	QString computeShaderSource = preprocessShader(base + "geometry.comp");
	computeProgram->addCacheableShaderFromSourceCode(QOpenGLShader::Compute, computeShaderSource);
	computeProgram->link();
}


void ChunkWidget::initShadersFast() {
	const auto base = QString(":/3d/shaders/isosurface/");
	program = new QOpenGLShaderProgram(this);
	program->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, base + "geometry.vert");
	program->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, base + "geometry.frag");
	program->link();
	computeProgram = new QOpenGLShaderProgram(this);
	computeProgram->addCacheableShaderFromSourceFile(QOpenGLShader::Compute, base + "geometry.comp");
	computeProgram->link();
}


void ChunkWidget::initBuffers() {
	voxelBuffer.create();
	voxelBuffer.bind();
	voxelBuffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
	std::vector<float> voxels(mAxis * mAxis * mAxis * 15, 0.0f);
	voxelBuffer.allocate(voxels.data(), voxels.size() * sizeof(float));
	vao.create();
	vao.bind();
}

void ChunkWidget::runComputeShader() {
	voxelBuffer.bind();
	computeProgram->bind();
	computeProgram->setUniformValue("voxel_size", 0.15f);
	computeProgram->setUniformValue("threshold", std::sin((mTime.elapsed()/1000.0f) * 0.2f));
	glDispatchCompute(mAxis, mAxis, mAxis);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	computeProgram->release();
}

void ChunkWidget::renderVoxels() {
	program->bind();
	voxelBuffer.bind();
	glDrawArrays(GL_TRIANGLES, 0, mAxis * mAxis * mAxis * 15);
	program->release();
}

#endif //ANDROID
