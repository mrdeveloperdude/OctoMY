#ifndef BUFFER_HPP
#define BUFFER_HPP

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

template <typename T>
struct Buffer{
	size_t size;
	quint8 pad[4];
	T *buffer;

	explicit Buffer(size_t size);
	virtual ~Buffer();
	T operator[] (size_t index);
};


////

template <typename T>
Buffer<T>::Buffer(size_t size)
	: size(size)
	, buffer(OC_NEW T[size])
{
	OC_METHODGATE();
}

template <typename T>
Buffer<T>::~Buffer()
{
	OC_METHODGATE();
	delete[] buffer;
	buffer=nullptr;
	size=0;
}

template <typename T>
T Buffer<T>::operator[] (size_t index){
	OC_METHODGATE();
	return buffer[index];
}


#endif // BUFFER_HPP
