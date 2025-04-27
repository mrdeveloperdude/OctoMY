#include "Utils.hpp"

#include <QFile>
#include <QPainter>
#include <QDir>

#include <QMatrix4x4>
#include <QTextStream>


QImage dummyTexture(const QSize textureSize, const QString &text)
{
	QImage image(textureSize, QImage::Format_RGBA8888);
	const QRect r(QPoint(0, 0), textureSize);
	QPainter p(&image);
	p.fillRect(r, QGradient::DeepBlue);
	QFont font;
	font.setPointSize(24);
	p.setFont(font);
	p.drawText(r, text);
	p.end();
	return image;
}


QShader getShader(const QString &name)
{
	QFile f(name);
	//qDebug() << name<< "exists:"<<f.exists();
	if(f.open(QIODevice::ReadOnly)){
		qDebug() << "Read " << name << " into shader";
		return QShader::fromSerialized(f.readAll());
	}
	else{
		qWarning() << "Could not open " << name << " for reading shader";
	}
	return QShader();
}

void listResources(const QString &path) {
	QDir dir(path);
	if (!dir.exists()) {
		qWarning() << "Path does not exist:" << path;
		return;
	}
	for (const QString &subDir : dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
		listResources(path + subDir + "/");
	}
	for (const QString &file : dir.entryList(QDir::Files)) {
		qDebug() << "File:" << path + file;
	}
}


QTextStream &operator<<(QTextStream &stream, const QMatrix4x4 &matrix)
{
	stream << "[";
	for (int row = 0; row < 4; ++row) {
		stream << "[";
		for (int col = 0; col < 4; ++col) {
			stream << matrix(row, col);
			if (col < 3) {
				stream << ", ";
			}
		}
		stream << "]";
		if (row < 3) {
			stream << ", ";
		}
	}
	stream << "]";
	return stream;
}



QDebug operator<<(QDebug dbg, const std::array<float, 16> &arr)
{
	QDebugStateSaver saver(dbg);
	dbg.nospace() << "std::array<float,16> [\n";
	for (size_t i = 0; i < arr.size(); ++i) {
		dbg.nospace() << "\t" << arr[i];
		if ((i + 1) % 4 == 0) {
			dbg.nospace() << "\n";
		} else {
			dbg.nospace() << ", ";
		}
	}
	dbg.nospace() << "]";
	return dbg;
}

