/*
 A C-program for MT19937, with initialization improved 2002/1/26.
 Coded by Takuji Nishimura and Makoto Matsumoto.

 Before using, initialize the state by using init_genrand(seed)
 or init_by_array(init_key, key_length).

 Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 1. Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.

 3. The names of its contributors may not be used to endorse or promote
 products derived from this software without specific prior written
 permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


 Any feedback is very welcome.
 http://www.math.keio.ac.jp/matumoto/emt.html
 email: matumoto@math.keio.ac.jp
 */
#include "MersennePRNG.hpp"

// Return true if this source is dependant on any pseudo rng
inline bool MersennePRNG::hasPseudo() {
	return true;
}

// Initializes PRNG with a seed if needed
void MersennePRNG::init() {
	init(5489UL);
}

// Initializes PRNG with a seed if needed
void MersennePRNG::init(quint32 s) {
	mt[0] = s & 0xffffffffUL;
	for (mti = 1; mti < MT_N; mti++) {
		mt[mti] = (1812433253UL * (mt[mti - 1] ^ (mt[mti - 1] >> 30)) + mti);
		// See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. In the previous versions, MSBs of the seed affect only MSBs of the array mt[]. 2002/01/09 modified by Makoto Matsumoto
		mt[mti] &= 0xffffffffUL;
		//For >32 bit machines
	}
}


MersennePRNG::MersennePRNG()
	: mti(MT_N + 1)
{

}

//Initialize by an array with array-length, init_key is the array for initializing keys, key_length is its length
void MersennePRNG::init(quint32 init_key[], size_t key_length) {
	quint32 i, j, k;
	init(19650218UL);
	i = 1;
	j = 0;
	k = (MT_N > key_length ? MT_N : key_length);
	for (; k; k--) {
		//Non linear
		mt[i] = (mt[i] ^ ((mt[i - 1] ^ (mt[i - 1] >> 30)) * 1664525UL)) + init_key[j] + j;
		//For WORDSIZE > 32 machines
		mt[i] &= 0xffffffffUL;
		i++;
		j++;
		if (i >= MT_N) {
			mt[0] = mt[MT_N - 1];
			i = 1;
		}
		if (j >= key_length) j = 0;
	}
	for (k = MT_N - 1; k; k--) {
		//Non linear
		mt[i] = (mt[i] ^ ((mt[i - 1] ^ (mt[i - 1] >> 30)) * 1566083941UL)) - i;
		//For WORDSIZE > 32 machines
		mt[i] &= 0xffffffffUL;
		i++;
		if (i >= MT_N) {
			mt[0] = mt[MT_N - 1];
			i = 1;
		}
	}

	//MSB is 1; assuring non-zero initial array
	mt[0] = 0x80000000UL;
}

//Generates a random number on [0,0xffffffff]-interval
//This is the main generator function that all the others are based on
quint32 MersennePRNG::generateInt32(void) {
	unsigned long y;
	static unsigned long mag01[2] = { 0x0UL, MT_MATRIX_A };
	//mag01[x] = x * MT_MATRIX_A  for x=0,1
	//Generate MT_N words at one time
	if (mti >= MT_N) {
		int kk;
		// If Init() has not been called, a default initial seed is used
		// The program will fail unless an init call has been made.
		// (The default one is put in the Init that takes no parameters).
		// This is till left here because it seems to be part of the loop
		if (mti == MT_N + 1) init(5489UL);
		for (kk = 0; kk < MT_N - MT_M; kk++) {
			y = (mt[kk] & MT_UPPER_MASK) | (mt[kk + 1] & MT_LOWER_MASK);
			mt[kk] = mt[kk + MT_M] ^ (y >> 1) ^ mag01[y & 0x1UL];
		}
		for (; kk < MT_N - 1; kk++) {
			y = (mt[kk] & MT_UPPER_MASK) | (mt[kk + 1] & MT_LOWER_MASK);
			mt[kk] = mt[kk + (MT_M - MT_N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
		}
		y = (mt[MT_N - 1] & MT_UPPER_MASK) | (mt[0] & MT_LOWER_MASK);
		mt[MT_N - 1] = mt[MT_M - 1] ^ (y >> 1) ^ mag01[y & 0x1UL];
		mti = 0;
	}
	y = mt[mti++];
	//Tempering
	y ^= (y >> 11);
	y ^= (y << 7) & 0x9d2c5680UL;
	y ^= (y << 15) & 0xefc60000UL;
	y ^= (y >> 18);
	return y;
}

