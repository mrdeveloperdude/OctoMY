#include <QDebug>
#include <QDirIterator>
#include <QFile>
#include <QGuiApplication>
#include <QMap>
#include <QPainter>
#include <QStringList>
#include <QSvgRenderer>
#include <QTextStream>
#include <iostream>

/*
 * Class to capture the log output during loading and rendering of SVG files from disk.
 * Any log output should be captured and all files with errors should be listed in a table.
*/
class SVGChecker {
private:
	// Update fileErrors to use a map for counting identical entries
	QMap<QString, QMap<QString, int>> fileErrors;
	bool svgErrorDetected;
	QString mCurrentFilePath;
	static SVGChecker *instance;
	
public:
	SVGChecker();
	
public:
	void processDirectory(const QString &dirPath);
	void displaySummary() const;
	
	void processSvgFile(const QString &filePath);
	static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
	
private:
	void appendLogEntry(const QString &filePath, const QString &entry);
	void installMessageHandler();
	void restoreDefaultMessageHandler();
};

SVGChecker *SVGChecker::instance = nullptr;

SVGChecker::SVGChecker() : svgErrorDetected(false) {
	instance = this;
}

void SVGChecker::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
	Q_UNUSED(type);
	if (instance) {
		instance->appendLogEntry(instance->mCurrentFilePath, msg);
	}
	else{
		exit(1337);
	}
}

void SVGChecker::appendLogEntry(const QString &filePath, const QString &entry) {
	if(filePath.isEmpty()) {
		exit(69);
	}
	// Increment the count of the entry in the file-specific error map
	fileErrors[filePath][entry]++;
	svgErrorDetected = true;
}

void SVGChecker::installMessageHandler() {
	std::cout << std::flush;
	qInstallMessageHandler(SVGChecker::messageHandler);
}

void SVGChecker::restoreDefaultMessageHandler() {
	std::cout << std::flush;
	qInstallMessageHandler(nullptr);  // Restores the default handler
}

void SVGChecker::processSvgFile(const QString &filePath) {
	svgErrorDetected = false;
	mCurrentFilePath = filePath;
	installMessageHandler();
	QSvgRenderer renderer;
	if (!renderer.load(mCurrentFilePath)) {
		appendLogEntry(mCurrentFilePath, "Failed to load SVG");
		restoreDefaultMessageHandler();
		return;
	}
	QImage dummyImage(1, 1, QImage::Format_ARGB32);
	QPainter painter(&dummyImage);
	renderer.render(&painter);
	restoreDefaultMessageHandler();
}

void SVGChecker::processDirectory(const QString &dirPath) {
	QDirIterator it(dirPath, QStringList() << "*.svg", QDir::Files, QDirIterator::Subdirectories);
	while (it.hasNext()) {
		QString filePath = it.next();
		processSvgFile(filePath);
	}
}

void SVGChecker::displaySummary() const {
	int maxFilenameLength = 0;
	for (const QString &filename : fileErrors.keys()) {
		maxFilenameLength = qMax(maxFilenameLength, filename.length());
	}
	QTextStream out(stdout);
	for (const QString &filename : fileErrors.keys()) {
		out << QString("%1 : ").arg(filename.rightJustified(maxFilenameLength));
		// For each file, print the error message and the number of occurrences
		QMap<QString, int> errors = fileErrors[filename];
		for (const QString &errorMsg : errors.keys()) {
			out << QString("%1 (x%2), ").arg(errorMsg).arg(errors[errorMsg]);
		}
		out << "\n";
	}
}

int main(int argc, char *argv[]) {
	QGuiApplication app(argc, argv);
	if (argc < 2) {
		qCritical() << "Usage: svgchecker <directory>";
		return -1;
	}
	QString directoryPath = argv[1];
	QDir dir(directoryPath);
	
	if (!dir.exists()) {
		qCritical() << "Directory does not exist:" << directoryPath;
		return -1;
	}
	SVGChecker checker;
	checker.processDirectory(directoryPath);
	checker.displaySummary();
	return 0;
}
