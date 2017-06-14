#ifndef TETRAGPSENCODER_HPP
#define TETRAGPSENCODER_HPP

#include <QString>

typedef long double TGPSReal ;


class TetraGPSEncoder{
	private:
		TGPSReal planetRadiusMeters;
	public:
		explicit TetraGPSEncoder(TGPSReal planetRadiusMeters=6371000.0);
		QString generate(TGPSReal lat, TGPSReal lon, TGPSReal precMeters);

};

#endif // TETRAGPSENCODER_HPP
