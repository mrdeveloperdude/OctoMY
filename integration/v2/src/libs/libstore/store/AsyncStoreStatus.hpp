#ifndef ASYNCSTORESTATUS_HPP
#define ASYNCSTORESTATUS_HPP

#include <QtGlobal>

class AsyncStoreStatus
{
private:
	quint64 mDiskCounter;
	quint64 mMemoryCounter;

public:
	explicit AsyncStoreStatus( quint64 diskCounter=0, quint64 memoryCounter=0 );

	bool isVirgin();
	bool isReady();
	bool needsGenerate();
	bool needsLoad();
	bool diskIsMostRecent();
	bool memoryIsMostRecent();
	bool existsOnDisk();
	bool existsInMemory();

public:

	QString toString() const;
	operator QString() const;
	operator QString();


};

#endif // ASYNCSTORESTATUS_HPP
