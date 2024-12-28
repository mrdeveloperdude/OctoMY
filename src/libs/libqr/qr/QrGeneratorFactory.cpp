#include "QrGeneratorFactory.hpp"

#include "native/Encoder.hpp"
#include "uptime/New.hpp"


IQrGenerator *getQrGenerator(const QString &type){
	if("native" == type){
		return OC_NEW Encoder;
	}
	if("nayuki" == type){
		return OC_NEW Encoder;
	}
	return OC_NEW Encoder;
}
