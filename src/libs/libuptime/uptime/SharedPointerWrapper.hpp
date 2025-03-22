#ifndef SHAREDPOINTERWRAPPER_HPP
#define SHAREDPOINTERWRAPPER_HPP


// This is a wrapper around QSharedPointer header to allow a centralized
// way of enabling tracking features via QT_SHAREDPOINTER_TRACK_POINTERS

#define QT_SHAREDPOINTER_TRACK_POINTERS

#include   <QSharedPointer>


#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-const-variable"
#endif

inline int sharedPointerWrapperDummyFunction() {
	QSharedPointer<int> dummy;
	(void)dummy;
	return 0;
}

#if defined(__clang__)
#pragma clang diagnostic pop
#endif


#endif
// SHAREDPOINTERWRAPPER_HPP
