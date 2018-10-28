#include "Website.hpp"

#include "utility/Standard.hpp"
#include "utility/Utility.hpp"
#include "markdown/Markdown.hpp"

#include "template/Mustache.cpp"


#include <QDir>

//#include <QtConcurrent/QtConcurrentMap>
#include <QtConcurrent/QtConcurrent>




Website::Website( AppContext * appContext)
	: mAppContext (appContext)
{
	OC_METHODGATE();
}

void Website::run()
{
	OC_METHODGATE();

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

	QFuture<void> future = QtConcurrent::map(mSources, [=](QString source) {
		if(source.startsWith(inputDir)) {
			QString baseFile=source;
			baseFile.remove(0, idsz).replace(".md", ".html");
			const QString outFile=outputDir+baseFile;
			auto outFilePath=QFileInfo (outFile).dir();
			if(QDir().mkpath(outFilePath.absolutePath())){
				qDebug()<< "SAVING '"<< outFile << "' From '"<<source<<"'";
				Markdown markdown;

				Mustache::Renderer renderer;
				Mustache::QtVariantContext context(mustacheData);
				utility::stringToFile(outFile, markdown.process(renderer.render(utility::fileToString(source), &context)));
			}
			else{
				qDebug()<<"ERROR: Outpuf file path '"<<outFilePath<<"' did not exist or could not be created";
			}


		}
	});
	future .waitForFinished();
}


void Website::updateSourcesList(QDir dir)
{
	dir.setNameFilters(QStringList("*.md"));
	dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

	qDebug() << "Scanning: " << dir.path();

	QStringList fileList = dir.entryList();
	bool foundIndex=false;
	for (int i=0; i<fileList.count(); i++) {
		QString file=fileList[i];
		const bool isIndex=(file=="index.md");
		qDebug() << " + Found "<< (isIndex?"index:":"file: ") << file;
		mSources << dir.absoluteFilePath(file);
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
