#ifndef HAARLOADER_HPP
#define HAARLOADER_HPP

#include <QString>
#include <QSharedPointer>
#include "HaarCascade.hpp"

class HaarLoader {
public:
	virtual ~HaarLoader() = default;
	
	// Pure virtual function to load a Haar cascade from a source
	virtual QSharedPointer<HaarCascade> load(const QString &source) = 0;
};

#endif // HAARLOADER_HPP

