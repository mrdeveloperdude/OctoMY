#ifndef RNG_HPP
#define RNG_HPP


#include <QColor>

#include <QtMath>

// Classification of RNG
enum RNGMerit {
	// For applications that just need random numbers in a hurry
	EFFICIENT,
	// For applications that needs ALOT of random data
	SCALABLE,
	// For security applications that need unpredictable random numbers
	SECURE,
	// For scientific applications that need good random numbers
	ACCURATE,
	// For applications that want the system default
	SYSTEM,
	// For applications that don't really care. This is the default/fallback
	GENERAL
};

// Random number generator baseclass
class RNG
{
private:
	QString name;

protected:
	bool ok;

public:

	//Ctor
	explicit RNG(QString name);

	//Dtor
	virtual ~RNG();

public:
	// Initializes entropysouce with a seed if needed
	virtual void init(quint32 s)=0;

	// Initialize by an array with array-length init_key is the array for
	// initializing keys key_length is its length
	virtual void init(quint32 init_key[], size_t key_length);

	// Initializes the (P)RNG wihtout seed (those that need a seed will default
	//   to lowest valid positive seed, such as 0 or 1, or the default specified
	//   for that particular generator)
	inline virtual void init();

	// Convenience wrapper to initialize by a bytearray.
	// Uses init(quint32 init_key[], size_t key_length); under the hood
	void init(QByteArray key);


	// Convenience wrapper to initialize by a QVector<quint32>.
	// Uses init(quint32 init_key[], size_t key_length); under the hood
	void init(QVector<quint32> key);


	// Generates a random number on [0,0xffffffff]-interval
	// This is the main generator function that all the others are based on
	virtual quint32 generateInt32(void)=0;

	// Generates a random number on [0,0x7fffffff]-interval
	inline quint32 generateInt31(void)
	{
		return static_cast<quint32> (generateInt32() >> 1);
	}

	// Generates a random number on [0,1]-real-interval
	inline double generateReal1(void)
	{
		return generateInt32() * (1.0 / 4294967295.0);
		//Divided by 2^32-1
	}

	// Generates a random number on [0,1)-real-interval
	inline double generateReal2(void)
	{
		return generateInt32() * (1.0 / 4294967296.0);
		//Divided by 2^32
	}

	// Generates a random number on (0,1)-real-interval
	inline double generateReal3(void)
	{
		return ((static_cast<double>(generateInt32())) + 0.5) * (1.0 / 4294967296.0);
		//Divided by 2^32
	}

	// Generates a random number on [0,1) with 53-bit resolution
	inline double generateReal53(void)
	{
		quint32 a = generateInt32() >> 5, b = generateInt32() >> 6;
		return (a * 67108864.0 + b) * (1.0 / 9007199254740992.0);
	}



	inline double generateGauss(void)
	{
#define RNG_GAUSS_NSUM 25

		double x = 0;
		int i;
		for(i = 0; i < RNG_GAUSS_NSUM; i++) {
			x += generateReal1();
		}

		x -= RNG_GAUSS_NSUM / 2.0;
		x /= std::sqrt(RNG_GAUSS_NSUM / 12.0);
#undef RNG_GAUSS_NSUM
		return x;
	}
	// NOTE: These routines are vital to deterministic code down stream, so the order/number/type of random numbers generated must not change.
	//       If you feel such a change is needed, create a new method beside this one.
	inline QColor generateQColorRGB()
	{
		QRgb rgb=generateInt32();
		return QColor(qRed(rgb), qGreen(rgb), qBlue(rgb));
	}

	// NOTE: These routines are vital to deterministic code down stream, so the order/number/type of random numbers generated must not change.
	//       If you feel such a change is needed, create a new method beside this one.
	inline QColor generateQColorRGBA()
	{
		QRgb rgba=generateInt32();
		return QColor(qRed(rgba), qGreen(rgba), qBlue(rgba), qAlpha(rgba));
	}

	// NOTE: These routines are vital to deterministic code down stream, so the order/number/type of random numbers generated must not change.
	//       If you feel such a change is needed, create a new method beside this one.
	inline QRgb generateQRgb()
	{
		return generateInt32()&0xFFFFFF00;
	}

	// NOTE: These routines are vital to deterministic code down stream, so the order/number/type of random numbers generated must not change.
	//       If you feel such a change is needed, create a new method beside this one.
	inline QRgb generateQRgba()
	{
		return generateInt32();
	}

	// Generate a color within ranges. Defaults to fully saturated, fully opaque random hue
	// NOTE: These routines are vital to deterministic code down stream, so the order/number/type of random numbers generated must not change.
	//       If you feel such a change is needed, create a new method beside this one.
	inline QColor generateRangedQColorHSLA(qreal hueLow=0.0, qreal hueHigh=1.0, qreal saturationLow=1.0, qreal saturationHigh=1.0, qreal luminanceLow=0.5, qreal luminanceHigh=0.5, qreal alphaLow=1.0, qreal alphaHigh=1.0)
	{
		// Clamp to legal range
		hueLow=qBound(0.0, hueLow, 1.0);
		saturationLow=qBound(0.0, saturationLow, 1.0);
		luminanceLow=qBound(0.0, luminanceLow, 1.0);
		alphaLow=qBound(0.0, alphaLow, 1.0);
		hueHigh=qBound(0.0, hueHigh, 1.0);
		saturationHigh=qBound(0.0, saturationHigh, 1.0);
		luminanceHigh=qBound(0.0, luminanceHigh, 1.0);
		alphaHigh=qBound(0.0, alphaHigh, 1.0);
		// Make sure low < high
		hueLow=qMin(hueHigh,hueLow);
		saturationLow=qMin(saturationHigh,saturationLow);
		luminanceLow=qMin(luminanceHigh,luminanceLow);
		alphaLow=qMin(alphaHigh,alphaLow);
		// Calculate ranges
		const qreal hueRange=hueHigh-hueLow;
		const qreal saturationRange=saturationHigh-saturationLow;
		const qreal luminanceRange=luminanceHigh-luminanceLow;
		const qreal alphaRange=alphaHigh-alphaLow;
		// Generate and return the color
		return QColor::fromHslF(hueLow+generateReal1()*hueRange, saturationLow+generateReal1()*saturationRange, luminanceLow+generateReal1()*luminanceRange, alphaLow+generateReal1()*alphaRange);
	}


	// Return true if this source is dependant on any pseudo rng
	virtual bool hasPseudo()=0;

	// Return name of this (P)RNG implementation
	inline QString getName()
	{
		return name;
	}

	// Return stratus of ths (P)RNG implementation
	inline bool isOk()
	{
		return ok;
	}

	// Returns true if this is an PRNG as opposed to an RNG (i.e. will produce
	// a deterministic sequence of output given the same seed)
	//	virtual bool isDeterministic(void)=0;


	// Return true if this source is dependant on any special purpose hardware
	// or api that may block execution inline virtual bool CanBlock()=0;

	// Entropy source factory. Provides new entropy sources based on desired merit
	static RNG * sourceFactory(RNGMerit m);

	// Entropy source factory. Provides new entropy sources based on named implementation
	static RNG * sourceFactory(QString name);

};



#endif
//RNG_HPP
