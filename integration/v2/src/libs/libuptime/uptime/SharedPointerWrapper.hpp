#ifndef SHAREDPOINTERWRAPPER_HPP
#define SHAREDPOINTERWRAPPER_HPP


// This is a wrapper around QSharedPointer header to allow a centralized
// way of enabling tracking features via QT_SHAREDPOINTER_TRACK_POINTERS

#define QT_SHAREDPOINTER_TRACK_POINTERS

#include <QSharedPointer>


#endif
// SHAREDPOINTERWRAPPER_HPP
