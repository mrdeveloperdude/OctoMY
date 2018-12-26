#ifndef MEMTRASHER_HPP
#define MEMTRASHER_HPP

#include "utility/string/String.hpp"

#include <QDateTime>
#include <QDebug>
#include <malloc.h>

//#define MAX_STRESS_MEM (1024L*1024L*1024L*31L)

quint64 totalAvailableMemory();

class MEMTrasher
{
private:
	char *stressMEM;
	const quint64 available;

public:
	explicit MEMTrasher()
		: stressMEM(nullptr)
		, available(static_cast<quint64>(findAvailableMem(1024, totalAvailableMemory())))
	{
	}

private:
	// Adaptively find available memory to trash, and return 90% of that to leave a margin.
	static qint64 findAvailableMem(quint64 MINREQ, quint64 required);

public:
	inline void adaptiveTrashMEM(qint64 pct)
	{

		//qDebug()<<"TRASHING!!!!!";
		if(nullptr != stressMEM) {
			free (stressMEM);
			stressMEM=nullptr;
			//qDebug() <<"FREE";
		}
		const qint64 bytes=((static_cast<quint64>(pct)*available)/100L);
		if(bytes>0) {
			//qDebug()<<"Trying stress mem "<<utility::humanReadableSize(available)<< " for pct="<<pct<< " for 100L="<<100L <<" for sizeof(char)="<<sizeof(char) <<" for available="<<available <<" for bytes="<<bytes ;
			stressMEM=static_cast<char *>(malloc(static_cast<size_t>(bytes)));
			int seed=rand();
			if(nullptr != stressMEM) {
				//Simulate using memory to avoid clever sparse virtual memory algorithms
				const qint64 typesize = sizeof(char);
				const qint64 arraySize=bytes/typesize;
				for(qint64 i=0; i<arraySize; i+=512) {
					stressMEM[i]=static_cast<char>((seed++)%0xff);
				}
				//qDebug() <<"STRESS-ALLOC "<<amt << " bytes";
			} else {
				qWarning() <<"ERROR: STRESS-ALLOC "<<utility::string::humanReadableSize(bytes) << " failed";
			}
		}
	}


};

#endif // MEMTRASHER_HPP

