#ifndef ASSERT_HPP
#define ASSERT_HPP

#include <QDebug>
#define OC_ASSERT(a) { const bool c=(a); if(!c){ qWarning()<< "OC_ASSERT FAILED: " #a; } }


#endif
// ASSERT_HPP
