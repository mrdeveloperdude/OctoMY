#include "Website.hpp"

#include "utility/Standard.hpp"

#include <QDir>

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
		mSources << file;
		foundIndex= foundIndex || isIndex;
	}
	if(!foundIndex){
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
