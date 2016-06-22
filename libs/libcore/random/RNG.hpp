#ifndef RNG_HPP
#define RNG_HPP

#include "basic/Standard.hpp"

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
class RNG {
	private:

		QString name;

	public:

		//Ctor
		explicit RNG(QString name);

		//Dtor
		virtual ~RNG();

		// Initializes the (P)RNG wihtout seed (those that need a seed will default
		//   to lowest valid positive seed, such as 0 or 1, or the default specified
		//   for that particular generator)
		inline virtual void init() {
			init(0);
		}

		// Initializes entropysouce with a seed if needed
		virtual void init(quint32 s)=0;

		// Initialize by an array with array-length init_key is the array for
		// initializing keys key_length is its length
		virtual void init(quint32 init_key[], size_t key_length);

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
		inline quint32 generateInt31(void) {
			return (quint32) (generateInt32() >> 1);
		}

		// Generates a random number on [0,1]-real-interval
		inline double generateReal1(void) {
			return generateInt32() * (1.0 / 4294967295.0);
			//Divided by 2^32-1
		}

		// Generates a random number on [0,1)-real-interval
		inline double generateReal2(void) {
			return generateInt32() * (1.0 / 4294967296.0);
			//Divided by 2^32
		}

		// Generates a random number on (0,1)-real-interval
		inline double generateReal3(void) {
			return (((double) generateInt32()) + 0.5) * (1.0 / 4294967296.0);
			//Divided by 2^32
		}

		// Generates a random number on [0,1) with 53-bit resolution
		inline double generateReal53(void) {
			quint32 a = generateInt32() >> 5, b = generateInt32() >> 6;
			return (a * 67108864.0 + b) * (1.0 / 9007199254740992.0);
		}

		// Return true if this source is dependant on any pseudo rng
		virtual bool hasPseudo()=0;

		// Return name of this (P)RNG implementation
		inline QString getName(){
			return name;
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
