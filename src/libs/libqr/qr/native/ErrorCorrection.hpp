#ifndef ERRORCORRECTION_HPP
#define ERRORCORRECTION_HPP


#include <QByteArray>
#include <QString>


/**
 * @brief The ErrorCorrection class is a Reed Solomon error correcting code
 * generator used in QR code encoding.
 */
class ErrorCorrection final {
private:
	// Coefficients of the divisor polynomial, stored from highest to lowest power, excluding the leading term which is always 1.
	QByteArray mCoefficients;
	QString mErrorString;
	
public:
	// Creates a Reed-Solomon ECC generator for the given degree.
	ErrorCorrection(int degree);
	
	// Computes and returns the Reed-Solomon error correction codewords for the given sequence of data codewords.
	// The returned object is always a new byte array. This method does not alter this object's state (because it is immutable).
	QByteArray getRemainder(const QByteArray &data) const;
	QString errorString();
	
private:
	bool fail(const QString &reason);
	
	// Returns the product of the two given field elements modulo GF(2^8/0x11D).
	// The arguments and result are unsigned 8-bit integers.
	static uint8_t multiply(uint8_t x, uint8_t y);
};



/**
 * @brief The Ecc class represents the error correction level used in a QR Code symbol.
 */
struct Ecc final {
	// (Public) In the range 0 to 3 (unsigned 2-bit integer).
	const int ordinal;
	// (Package-private) In the range 0 to 3 (unsigned 2-bit integer).
	const int formatBits;
	
	Ecc(int ord, int fb) :
		ordinal(ord),
		formatBits(fb) {}
};



extern Ecc ECC_LOW;
extern Ecc ECC_MEDIUM;
extern Ecc ECC_QUARTILE;
extern Ecc ECC_HIGH;

extern const int16_t NUM_ERROR_CORRECTION_CODEWORDS[4][41];
extern const int8_t NUM_ERROR_CORRECTION_BLOCKS[4][41];

#endif // ERRORCORRECTION_HPP
