#ifndef QRGENERATORFACTORY_HPP
#define QRGENERATORFACTORY_HPP

#include <QString>

#include "qr/IQrGenerator.hpp"

IQrGenerator *getQrGenerator(const QString &type = QString());

#endif // QRGENERATORFACTORY_HPP
