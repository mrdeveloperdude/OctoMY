#ifndef MEMTRASHER_HPP
#define MEMTRASHER_HPP
#include "utility/Utility.hpp"
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
		: stressMEM(0)
		, available(findAvailableMem(1024, totalAvailableMemory()))
	{
	}

private:
	//Adaptively find available memory to trash, and return 90% of that to leave a margin.
	static qint64 findAvailableMem(quint64 MINREQ, quint64 required)
	{
		qDebug()<<"LOOKING FOR AVAILABLE MEMORY WITH " <<utility::humanReadableSize(MINREQ)<<utility::humanReadableSize(required);
		void *mem = 0;
		while (0 == mem) {
			if (required < MINREQ) {
				return 0;
			}
			qDebug()<<"Trying " <<utility::humanReadableSize(required);
			mem = malloc (required);
			required=(required*90)/100;
		}
		free (mem);

		mem = malloc (required);
		if (mem == NULL) {
			qWarning()<<"Cannot enough allocate memory";
			return 0;
		}
		free (mem);
		qDebug()<<"FOUND AVAILABLE MEMORY TO BE " <<utility::humanReadableSize(required);
		return required;
	}

public:
	inline void adaptiveTrashMEM(int pct)
	{

		//qDebug()<<"TRASHING!!!!!";
		if(0!=stressMEM) {
			free (stressMEM);
			stressMEM=0;
			//qDebug() <<"FREE";
		}
		const qint64 bytes=((pct*available)/100L);
		if(bytes>0) {
			//qDebug()<<"Trying stress mem "<<utility::humanReadableSize(available)<< " for pct="<<pct<< " for 100L="<<100L <<" for sizeof(char)="<<sizeof(char) <<" for available="<<available <<" for bytes="<<bytes ;
			stressMEM=(char *)malloc(bytes);
			int seed=rand();
			if(0!=stressMEM) {
				//Simulate using memory to avoid clever sparse virtual memory algorithms
				const qint64 typesize=sizeof(char);
				const qint64 arraySize=bytes/typesize;
				for(qint64 i=0; i<arraySize; i+=512) {
					stressMEM[i]=(seed++)%0xff;
				}
				//qDebug() <<"STRESS-ALLOC "<<amt << " bytes";
			} else {
				qWarning() <<"ERROR: STRESS-ALLOC "<<utility::humanReadableSize(bytes) << " failed";
			}
		}
	}


};

#endif // MEMTRASHER_HPP

