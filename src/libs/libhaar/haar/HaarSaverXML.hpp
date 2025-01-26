#ifndef HAARSAVERXML_HPP
#define HAARSAVERXML_HPP

#include "HaarSaver.hpp"

class HaarSaverXML : public HaarSaver {
public:
	// Override the save function to write Haar cascade to an XML file
	void save(const HaarCascade &cascade, const QString &destination) override;
};

#endif // HAARSAVERXML_HPP

