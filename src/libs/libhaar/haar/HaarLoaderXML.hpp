#ifndef HAARLOADERXML_HPP
#define HAARLOADERXML_HPP

#include "HaarLoader.hpp"

class HaarLoaderXML : public HaarLoader {
private:
	bool mDebug{false};
public:
	// Override the load function to parse Haar cascade from XML
	QSharedPointer<HaarCascade> load(const QString &source) override;
};

#endif // HAARLOADERXML_HPP
