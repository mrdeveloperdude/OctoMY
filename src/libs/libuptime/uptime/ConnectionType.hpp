#ifndef CONNECTIONTYPE_HPP
#define CONNECTIONTYPE_HPP

#define OC_CONTYPE static_cast<Qt::ConnectionType> (Qt::QueuedConnection | Qt::UniqueConnection)
#define OC_CONTYPE_NON_UNIQUE static_cast<Qt::ConnectionType> (Qt::QueuedConnection)


#endif
// CONNECTIONTYPE_HPP
