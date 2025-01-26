#ifndef HAARSAVER_HPP
#define HAARSAVER_HPP

#include <QString>
#include "HaarCascade.hpp"

class HaarSaver {
public:
	virtual ~HaarSaver() = default;
	virtual void save(const HaarCascade &cascade, const QString &destination) = 0;
};

#endif // HAARSAVER_HPP
