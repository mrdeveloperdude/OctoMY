#include "Code.hpp"

#include <QDebug>

#include "Helpers.hpp"

// Returns a new byte string representing the given data with the appropriate error correction
// codewords appended to it, based on this object's version and error correction level.
QByteArray Code::appendErrorCorrection(const QByteArray &data){
	if (data.size() != static_cast<unsigned int>(numDataCodewords(version, errorCorrectionLevel))){
		fail("Invalid argument");
		return QByteArray();
	}
	
	// Calculate parameter numbers
	int numBlocks = NUM_ERROR_CORRECTION_BLOCKS[errorCorrectionLevel.ordinal][version];
	int totalEcc = NUM_ERROR_CORRECTION_CODEWORDS[errorCorrectionLevel.ordinal][version];
	if (totalEcc % numBlocks != 0){
		fail("Assertion error");
		return QByteArray();
	}
	int blockEccLen = totalEcc / numBlocks;
	int numShortBlocks = numBlocks - numRawDataModules(version) / 8 % numBlocks;
	int shortBlockLen = numRawDataModules(version) / 8 / numBlocks;
	
	// Split data into blocks and append ECC to each block
	QVector<QByteArray>  blocks;
	const ErrorCorrection rs(blockEccLen);
	for (int i = 0, k = 0; i < numBlocks; i++) {
		QByteArray dat;
		dat.insert(0, data.mid(k, shortBlockLen - blockEccLen + (i < numShortBlocks ? 0 : 1)));
		k += dat.size();
		const QByteArray ecc(rs.getRemainder(dat));
		if (i < numShortBlocks) {
			dat.append(static_cast<char>(0));
		}
		dat.append(ecc);
		blocks.append(dat);
	}
	
	// Interleave (not concatenate) the bytes from every block into a single sequence
	QByteArray result;
	for (int i = 0; static_cast<unsigned int>(i) < blocks.at(0).size(); i++) {
		for (int j = 0; static_cast<unsigned int>(j) < blocks.size(); j++) {
			// Skip the padding byte in short blocks
			if (i != shortBlockLen - blockEccLen || j >= numShortBlocks){
				result.push_back(blocks.at(j).at(i));
			}
		}
	}
	if (result.size() != static_cast<unsigned int>(numRawDataModules(version) / 8)){
		fail("Assertion error");
		return QByteArray();
	}
	return result;
}


int Code::handleConstructorMasking(int mask) {
	if (mask == -1) {  // Automatically choose best mask
		int32_t minPenalty = INT32_MAX;
		for (int i = 0; i < 8; i++) {
			drawFormatBits(i);
			applyMask(i);
			int penalty = mBitmap.penaltyScore();
			if (penalty < minPenalty) {
				mask = i;
				minPenalty = penalty;
			}
			applyMask(i);  // Undoes the mask due to XOR
		}
	}
	if (mask < 0 || mask > 7){
		fail("Assertion error");
		return -1;
	}
	drawFormatBits(mask);  // Overwrite old format bits
	applyMask(mask);  // Apply the final choice of mask
	return mask;  // The caller shall assign this value to the final-declared field
}



bool Code::drawCodewords(const QByteArray &data) {
	if (data.size() != static_cast<unsigned int>(numRawDataModules(version) / 8)){
		return fail("Invalid argument");
	}
	const auto size = width();
	qsizetype i = 0;  // Bit index into the data
	// Do the funny zigzag scan
	for (int right = size - 1; right >= 1; right -= 2) {  // Index of right column in each column pair
		if (right == 6){
			right = 5;
		}
		for (int vert = 0; vert < size; vert++) {  // Vertical counter
			for (int j = 0; j < 2; j++) {
				int x = right - j;  // Actual x coordinate
				bool upwards = ((right & 2) == 0) ^ (x < 6);
				int y = upwards ? size - 1 - vert : vert;  // Actual y coordinate
				if (!mFunctionmap.getPixel(x,y ) && i < data.size() * 8) {
					mBitmap.setPixel(x, y, ((data.at(i >> 3) >> (7 - (i & 7))) & 1) != 0);
					i++;
				}
				// If there are any remainder bits (0 to 7), they are already
				// set to 0/false/white when the grid of modules was initialized
			}
		}
	}
	if (static_cast<unsigned int>(i) != data.size() * 8){
		return fail("Assertion error");
	}
	return true;
}




Code::Code(const Ecc &ecl, int version, int mask, const QByteArray &dataCodewords)
	: version(version)
	, errorCorrectionLevel(ecl)
	, mask(mask)
{
	// Calculate bit pattern widt
	mBitmap.setWidth((1 <= version && version <= 40 ? version * 4 + 17 : -1));
	mFunctionmap.setWidth(mBitmap.width());
	// Draw function patterns, draw all codewords, do masking
	drawFunctionPatterns();
	const auto allCodewords = appendErrorCorrection(dataCodewords);
	drawCodewords(allCodewords);
	this->mask = handleConstructorMasking(mask);
}



bool Code::fail(const QString &reason){
	mErrorString = reason;
	return (!mErrorString.isNull()) || (!mErrorString.isEmpty());
}


void Code::setFunctionModule(int x, int y, bool value) {
	mBitmap.setPixel(x, y, value);
	mFunctionmap.setPixel(x, y, true);
}


bool Code::drawFunctionPatterns() {
	const auto size = mBitmap.width();
	// Draw the horizontal and vertical timing patterns
	for (int i = 0; i < size; i++) {
		setFunctionModule(6, i, i % 2 == 0);
		setFunctionModule(i, 6, i % 2 == 0);
	}
	
	// Draw 3 finder patterns (all corners except bottom right; overwrites some timing modules)
	drawFinderPattern(3, 3);
	drawFinderPattern(size - 4, 3);
	drawFinderPattern(3, size - 4);
	
	// Draw the numerous alignment patterns
	const std::vector<int> alignPatPos(alignmentPatternPositions(version));
	int numAlign = alignPatPos.size();
	for (int i = 0; i < numAlign; i++) {
		for (int j = 0; j < numAlign; j++) {
			if ((i == 0 && j == 0) || (i == 0 && j == numAlign - 1) || (i == numAlign - 1 && j == 0)){
				continue;  // Skip the three finder corners
			}
			else {
				drawAlignmentPattern(alignPatPos.at(i), alignPatPos.at(j));
			}
		}
	}
	
	// Draw configuration data
	drawFormatBits(0);  // Dummy mask value; overwritten later in the constructor
	drawVersion();
	return true;
}


bool Code::drawFormatBits(int mask) {
	const auto size = mBitmap.width();
	// Calculate error correction code and pack bits
	int data = errorCorrectionLevel.formatBits << 3 | mask;  // errCorrLvl is uint2, mask is uint3
	int rem = data;
	for (int i = 0; i < 10; i++)
		rem = (rem << 1) ^ ((rem >> 9) * 0x537);
	data = data << 10 | rem;
	data ^= 0x5412;  // uint15
	if (data >> 15 != 0){
		return fail("Assertion error");
	}
	
	// Draw first copy
	for (int i = 0; i <= 5; i++)
		setFunctionModule(8, i, ((data >> i) & 1) != 0);
	setFunctionModule(8, 7, ((data >> 6) & 1) != 0);
	setFunctionModule(8, 8, ((data >> 7) & 1) != 0);
	setFunctionModule(7, 8, ((data >> 8) & 1) != 0);
	for (int i = 9; i < 15; i++)
		setFunctionModule(14 - i, 8, ((data >> i) & 1) != 0);
	
	// Draw second copy
	for (int i = 0; i <= 7; i++){
		setFunctionModule(size - 1 - i, 8, ((data >> i) & 1) != 0);
	}
	for (int i = 8; i < 15; i++){
		setFunctionModule(8, size - 15 + i, ((data >> i) & 1) != 0);
	}
	setFunctionModule(8, size - 8, true);
	return true;
}


bool Code::drawVersion() {
	const auto size = mBitmap.width();
	if (version < 7){
		return true;
	}
	
	// Calculate error correction code and pack bits
	int rem = version;  // version is uint6, in the range [7, 40]
	for (int i = 0; i < 12; i++){
		rem = (rem << 1) ^ ((rem >> 11) * 0x1F25);
	}
	int data = version << 12 | rem;  // uint18
	if (data >> 18 != 0){
		return fail("Assertion error");
	}
	
	// Draw two copies
	for (int i = 0; i < 18; i++) {
		bool bit = ((data >> i) & 1) != 0;
		int a = size - 11 + i % 3, b = i / 3;
		setFunctionModule(a, b, bit);
		setFunctionModule(b, a, bit);
	}
	return true;
}


bool Code::drawFinderPattern(int x, int y) {
	const auto size = mBitmap.width();
	for (int i = -4; i <= 4; i++) {
		for (int j = -4; j <= 4; j++) {
			int dist = std::max(std::abs(i), std::abs(j));  // Chebyshev/infinity norm
			int xx = x + j, yy = y + i;
			if (0 <= xx && xx < size && 0 <= yy && yy < size){
				setFunctionModule(xx, yy, dist != 2 && dist != 4);
			}
		}
	}
	return true;
}



bool Code::drawAlignmentPattern(int x, int y) {
	for (int i = -2; i <= 2; i++) {
		for (int j = -2; j <= 2; j++){
			setFunctionModule(x + j, y + i, std::max(std::abs(i), std::abs(j)) != 1);
		}
	}
	return true;
}


//

QString Code::toSVG() const {
	return QString();
}

QImage Code::toImage() const {
	return QImage();
}


QString Code::errorString() const {
	return mErrorString;
}



QR_WIDTH_TYPE Code::width() const {
	return mBitmap.width();
}

bool Code::module(QR_WIDTH_TYPE x, QR_WIDTH_TYPE y) const{
	const auto w = width();
	if(x < 0 || x >= w){
		qWarning() << "Trying to get module out of horizontal range";
		return false;
	}
	if(y < 0 || y >= w){
		qWarning() << "Trying to get module out of vertical range";
		return false;
	}
	return mBitmap.getPixel(x, y);
}

