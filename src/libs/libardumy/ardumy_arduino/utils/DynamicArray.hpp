#ifndef DYNAMICARRAY_HPP
#define DYNAMICARRAY_HPP

#include "ardumy_arduino/ArduMY.hpp"
/*

This is meant to be a dead simple, MCU friendly dynamic array.

Its main traits are:
 1. Works correctly and as epxected
 2. As greedy on memory as possible

 There are no bytes wasted and performance is not cared for at all.

*/

template<class T>
class DynamicArray
{

private:
	T *mData;
	uint32_t mSize;


public:
	explicit DynamicArray(uint32_t initialSize=0);
	DynamicArray(const DynamicArray &a);
	virtual ~DynamicArray();


public:

	DynamicArray& operator = (const DynamicArray &a);
	template <typename I>
	T& operator [] (I index);
	template <typename I>
	const T& operator [] (I index) const;

public:

	uint32_t size() const;
	void setSize(uint32_t newsize);
	void remove(uint32_t id);
	void clear();
	T* data();

};


//////////////////////////////////////////////////////////


template <class T>
DynamicArray<T>::DynamicArray(uint32_t initialSize)
	: mData(nullptr)
	, mSize(initialSize)
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
	: DynamicArray(0)// Invoke default constructor
{
	// Invoke operator=
	*this=a;
}


template <class T>
DynamicArray<T>& DynamicArray<T>::operator = (const DynamicArray &a)
{
	if (this == &a) {
		return *this;
	}
	setSize(a.mSize);
	//memcpy(mData, a.mData, sizeof(T)*a.mSize);
	for(uint32_t i=0; i<a.mSize; ++i) {
		mData[i]=a.mData[i];
	}
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
	if ((newsize != mSize) || (0==mSize) ) {
		const uint32_t oldSize=mSize;
		const uint32_t copySize=((oldSize < newsize) ? oldSize : newsize);
		T *tmp= new T[newsize];
		uint32_t i=0;
		if(nullptr!=mData) {
			//qDebug()<<"Copied: "<<(copySize);
			for(; i<copySize; ++i) {
				tmp[i]=mData[i];
			}
			delete [] mData;
			mData=nullptr;
		}
		//qDebug()<<"Initialized: "<<(newsize-i);
		for(; i<newsize; ++i) {
			T &t=tmp[i];
			t=T(); //Value-initialize (set to 0) all items beyond the copy point.
		}
		mSize = newsize;
		mData = tmp;
		tmp = nullptr;
	}
}

template <class T>
void DynamicArray<T>::remove(uint32_t id)
{
	if(id>=mSize) {
		return;
	}
	if(mSize<=0) {
		return;
	}
	const uint32_t newsize=mSize-1;
	//const uint32_t copySize=newsize;
	T *tmp= new T[newsize];
	uint32_t i=0;
	if(nullptr!=mData) {
		//qDebug()<<"Copied: "<<(copySize);
		for(; i<id; ++i) {
			tmp[i]=mData[i];
		}
		for(; i<newsize; ++i) {
			tmp[i]=mData[i+1];
		}
		delete [] mData;
		mData=nullptr;
	}
	//qDebug()<<"Initialized: "<<(newsize-i);
	else {
		for(; i<newsize; ++i) {
			T &t=tmp[i];
			t=T(); //Value-initialize (set to 0) all items beyond the copy point.
		}
	}
	mSize = newsize;
	mData = tmp;
	tmp = nullptr;
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
template <typename I>
T& DynamicArray<T>::operator [] (I index)
{
	return mData[static_cast<uint32_t>(index)];
}

template <class T>
template <typename I>
const T& DynamicArray<T>::operator [] ( I index) const
{
	return mData[static_cast<uint32_t>(index)];
}

#endif // DYNAMICARRAY_HPP



























