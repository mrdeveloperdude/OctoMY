#include "Website.hpp"


#include "uptime/MethodGate.hpp"
#include "utility/time/ScopedTimer.hpp"
#include "utility/string/String.hpp"
#include "utility/file/File.hpp"
#include "markdown/Markdown.hpp"

#include "template/Mustache.cpp"


#include <QDir>
#include <QDebug>
#include <QtConcurrent/QtConcurrent>


Website::Website( AppContext * appContext)
	: mAppContext (appContext)
{
	OC_METHODGATE();
}


void Website::run()
{
	OC_METHODGATE();
	ScopedTimer st("Website generation");

	if(nullptr==mAppContext) {
		qDebug()<<"ERROR: no app context, quitting...";
		return;
	}
	//auto &env=mAppContext->environment();
	auto &opts=mAppContext->options();

	QString outputDir=QDir::currentPath();
	if(opts.isSet("output-dir")) {
		outputDir=opts.value("output-dir");
	}

	if(!QDir().mkpath(outputDir)) {
		qDebug()<<"ERROR: output dir '"<< outputDir<< "' did not exist or could not be created, quitting...";
		return;
	}

	outputDir=QDir::cleanPath(outputDir);
	qDebug()<<"Using output dir '"<< outputDir<< "'";

	QString inputDir=QDir::currentPath();
	if(opts.isSet("input-dir")) {
		inputDir=opts.value("input-dir");
	}

	inputDir=QDir::cleanPath(inputDir);
	qDebug()<<"Using input dir '"<< inputDir<< "'";

	mSources.clear();
	updateSourcesList(QDir(inputDir));

	const auto idsz=inputDir.size();


	QVariantHash mustacheData;
	mustacheData["name"] = "John Smith";
	mustacheData["email"] = "john.smith@gmail.com";
	mustacheData["title"] = "Ready-to-run robot software";
	mustacheData["webroot"] = "/";

	QFuture<void> future = QtConcurrent::map(mSources, [=](QString source) {
		if(source.startsWith(inputDir)) {
			QString baseFile=source;
			QString sourceDir=QFileInfo(source).absolutePath();
			baseFile.remove(0, idsz).replace(".mustache", "");
			const QString outFile=outputDir+baseFile;
			auto outFilePath=QFileInfo (outFile).dir();
			if(QDir().mkpath(outFilePath.absolutePath())) {
				qDebug()<< "SAVING '"<< outFile << "' From '"<<source<<"'" << " ('"<<sourceDir <<"')";
				//Markdown markdown;
				PartialFileLoader partialLoader(sourceDir);
				Mustache::Renderer renderer;
				Mustache::QtVariantContext context(mustacheData
												   //);
												   , &partialLoader);
				//markdown.process(
				QString raw = utility::file::fileToString(source);
				QString processed=renderer.render(raw, &context);
				if(renderer.errorPos()>=0){
					qWarning()<<"MUSTACHE ERROR: "<< renderer.errorPartial()<<": "<<renderer.error();
				}
				else{
					utility::file::stringToFile(outFile, processed);
				}
			} else {
				qDebug()<<"ERROR: Output file path '"<<outFilePath<<"' did not exist or could not be created";
			}

		}
		else{
			qDebug()<<"Skipping "<<source;
		}
	});
	future .waitForFinished();
}


void Website::updateSourcesList(QDir dir)
{
	dir.setNameFilters(QStringList("*.mustache"));
	dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

	qDebug() << "Scanning: " << dir.path();

	QStringList fileList = dir.entryList();
	bool foundIndex=false;
	for (int i=0; i<fileList.count(); i++) {
		QString file=fileList[i];
		const bool isIndex=(file.startsWith("index."));
		qDebug() << " + Found "<< (isIndex?"index:":"file: ") << file;
		if(isIndex) {
			mSources << dir.absoluteFilePath(file);
		}
		foundIndex= foundIndex || isIndex;
	}
	if(!foundIndex) {
		//createIndex(dirList);
		qDebug() << " * No index found";
	}
	dir.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
	QStringList dirList = dir.entryList();
	for (int i=0; i<dirList.size(); ++i) {
		QString newPath = QString("%1/%2").arg(dir.absolutePath()).arg(dirList.at(i));
		updateSourcesList(QDir(newPath));
	}
}
