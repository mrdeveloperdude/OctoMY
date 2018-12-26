#ifndef UTILITY_FILE_HPP
#define UTILITY_FILE_HPP

#include <QString>
#include <QVariant>

namespace utility
{

namespace file
{

	QString fileToString(QString fn);
	QByteArray fileToByteArray(QString fn);
	QVariant jsonFileToVariant(QString fn);
	bool ensureDirExistsForFile(QString fn);
	bool variantToJsonFile(QString fn, QVariant data);
	bool stringToFile(QString fn, QString data, bool append=false);
	bool byteArrayToFile(QString fn, QByteArray data, bool append=false);
	bool verifyMedia(QString name);


}

}

#endif
// UTILITY_FILE_HPP

