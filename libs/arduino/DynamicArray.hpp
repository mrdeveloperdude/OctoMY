#ifndef DYNAMICARRAY_HPP
#define DYNAMICARRAY_HPP

#include "ArduMY.hpp"


template<class T>
class DynamicArray
{

private:
	T *mData;
	uint32_t mSize;
	uint32_t mActualSize;

	static const uint32_t mGrowStep = 4;
	static const uint32_t mGrowMultiplier = 2;

public:
	explicit DynamicArray(uint32_t initialSize=0);
	DynamicArray(const DynamicArray &a);
	virtual ~DynamicArray();


public:

	DynamicArray& operator = (const DynamicArray &a);
	T& operator [] (uint32_t index);
	const T& operator [] (uint32_t index) const;

public:

	uint32_t size() const;
	void setSize(uint32_t newsize);
	void clear();
	T* data();

};


//////////////////////////////////////////////////////////


template <class T>
DynamicArray<T>::DynamicArray(uint32_t initialSize)
	: mData(nullptr)
	, mSize(initialSize-1) // NOTE: don't set to initialSize here to avoid the trivial reject in setSize()
	, mActualSize(mGrowStep)
{
	setSize(initialSize);
}


template <class T>
DynamicArray<T>::~DynamicArray()
{
	delete [] (mData);
	mData = nullptr;
}


template <class T>
DynamicArray<T>::DynamicArray(const DynamicArray &a)
{
	mData = new T[a.mActualSize];
	memcpy(mData, a.mData, sizeof(T)*a.mActualSize);
	mActualSize = a.mActualSize;
	mSize = a.mSize;
}


template <class T>
DynamicArray<T>& DynamicArray<T>::operator = (const DynamicArray &a)
{
	if (this == &a) {
		return *this;
	}
	setSize(a.mSize);
	memcpy(mData, a.mData, sizeof(T)*a.mSize);
	return *this;
}

template <class T>
uint32_t DynamicArray<T>::size() const
{
	return mSize;
}

template <class T>
void DynamicArray<T>::setSize(uint32_t newsize)
{
	// Trivial reject
	if(newsize==mSize){
		return;
	}
	const uint32_t oldSize=mSize;
	mSize = newsize;
	if ((mSize > mActualSize) || (mSize < mActualSize/2)) {
		mActualSize = mSize;
		if(mActualSize<mGrowStep) {
			mActualSize=mGrowStep;
		}
		T *tmp= new T[mActualSize];
		if(nullptr!=mData) {
			memcpy(mData, tmp, sizeof(T)*oldSize);
			delete [] mData;
			mData=nullptr;
		}
		mData=tmp;
		tmp=nullptr;
	}

	// TODO: handle undefined values in cases where T is a fundamental type
}

template <class T>
void DynamicArray<T>::clear()
{
	setSize(0);
}

template <class T>
T* DynamicArray<T>::data()
{
	return mData;
}

template <class T>
T& DynamicArray<T>::operator [] (uint32_t index)
{
	return mData[index];
}

template <class T>
const T& DynamicArray<T>::operator [] (uint32_t index) const
{
	return mData[index];
}

#endif // DYNAMICARRAY_HPP
