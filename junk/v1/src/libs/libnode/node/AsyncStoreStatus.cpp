#include "AsyncStoreStatus.hpp"

#include "utility/Standard.hpp"

AsyncStoreStatus::AsyncStoreStatus(quint64 diskCounter, quint64 memoryCounter)
	: mDiskCounter(diskCounter)
	, mMemoryCounter(memoryCounter)
{

}


bool AsyncStoreStatus::isVirgin()
{
	OC_METHODGATE();
	return (0 == mMemoryCounter) && (0 == mDiskCounter);
}


bool AsyncStoreStatus::isReady()
{
	OC_METHODGATE();
	return (mMemoryCounter > 0);
}


bool AsyncStoreStatus::needsGenerate()
{
	OC_METHODGATE();
	return ( mMemoryCounter ==  mDiskCounter) && ( 0 == mDiskCounter);
}


bool AsyncStoreStatus::needsLoad()
{
	OC_METHODGATE();
	return ( mMemoryCounter < mDiskCounter) && ( 0 != mDiskCounter);
}


bool AsyncStoreStatus::diskIsMostRecent()
{
	OC_METHODGATE();
	return mDiskCounter > mMemoryCounter;
}


bool AsyncStoreStatus::memoryIsMostRecent()
{
	OC_METHODGATE();
	return mDiskCounter < mMemoryCounter;
}


bool AsyncStoreStatus::existsOnDisk()
{
	OC_METHODGATE();
	return mDiskCounter > 0;
}



bool AsyncStoreStatus::existsInMemory()
{
	OC_METHODGATE();
	return mMemoryCounter > 0;
}




QString AsyncStoreStatus::toString() const
{
	return QString("AsyncStoreStatus(disk=%1, mem=%2)").arg(mDiskCounter).arg(mMemoryCounter);
}


AsyncStoreStatus::operator QString() const
{
	return toString();
}


AsyncStoreStatus::operator QString()
{
	return toString();
}


