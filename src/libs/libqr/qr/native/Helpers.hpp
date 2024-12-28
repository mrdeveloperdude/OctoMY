#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <vector>

struct Ecc;

extern int numRawDataModules(int ver);
extern int numDataCodewords(int ver, const Ecc &ecl);
extern Ecc *calculateBoostedEcc(const int version, const int dataUsedBits, Ecc *ecl);
extern std::vector<int> alignmentPatternPositions(int ver);



#endif // HELPERS_HPP
