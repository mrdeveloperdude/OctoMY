#ifndef SECURITYCONSTANTS_HPP
#define SECURITYCONSTANTS_HPP

#include "../libpki/qpolarsslpki.hpp"


#include <QCryptographicHash>

// OctoMY™ sanctioned values for key-generation

extern const quint32 OCTOMY_KEY_BITS;
extern const QCryptographicHash::Algorithm OCTOMY_KEY_HASH;
extern const qpolarssl::THash OCTOMY_KEY_HASH_POLAR;



#endif // SECURITYCONSTANTS_HPP
