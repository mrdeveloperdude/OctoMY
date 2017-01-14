#ifndef DYNAMICARRAY_HPP
#define DYNAMICARRAY_HPP

#include "ArduMY.hpp"


template<class T>
class DynamicArray
{

private:
	T *mData;
	unsigned int mSize;
	unsigned int mActualSize;

	static const int mGrowStep = 4;
	static const int mGrowMultiplier = 2;

public:
	explicit DynamicArray(unsigned int initialSize=0);
	DynamicArray(const DynamicArray &a);
	virtual ~DynamicArray();


public:

	DynamicArray& operator = (const DynamicArray &a);
	T& operator [] (unsigned int index);
	const T& operator [] (unsigned int index) const;

public:

	unsigned int size() const;
	void setSize(unsigned int newsize);
	void clear();
	T* data();

};


//////////////////////////////////////////////////////////


template <class T>
DynamicArray<T>::DynamicArray(unsigned int initialSize)
	: mData(nullptr)
	, mSize(0)
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
unsigned int DynamicArray<T>::size() const
{
	return mSize;
}

template <class T>
void DynamicArray<T>::setSize(unsigned int newsize)
{
	const unsigned int oldSize=mSize;
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

	// TODO: handle undefined values in cases where T is a fundamentaltype
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
T& DynamicArray<T>::operator [] (unsigned int index)
{
	return mData[index];
}

template <class T>
const T& DynamicArray<T>::operator [] (unsigned int index) const
{
	return mData[index];
}

#endif // DYNAMICARRAY_HPP
