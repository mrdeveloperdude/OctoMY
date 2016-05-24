#ifndef ZOORECORDSTATE_HPP
#define ZOORECORDSTATE_HPP

#include <QtGlobal>

/**
 * Immutable class containing the state of a Zoo record
*/
class ZooRecordState{
	private:
		const quint16 flags;
	public:

		explicit ZooRecordState(bool keyExists, bool inMemory, bool onDisk, bool diskSynced)
			: flags( (keyExists?0:1) | (inMemory?0:2) | (onDisk?0:4) | (diskSynced?0:8) )
		{

		}

		virtual ~ZooRecordState(){

		}

		inline bool keyExists()	const {return (flags&1)>0;} // Is this record pointing to a valid piece of data?
		inline bool inMemory()	const {return (flags&2)>0;} // Is the latest incarnation of data for this record in memory?
		inline bool onDisk()	const {return (flags&4)>0;} // Is the latest incarnation of data for this record stored safely on disk?
		inline bool diskSynced()const {return (flags&8)>0;} // Has disk been synchronized since latest incarnation of data for this record was stored safely on disk?
};

#endif // ZOORECORDSTATE_HPP
