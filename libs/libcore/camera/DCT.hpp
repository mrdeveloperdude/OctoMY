#ifndef DCT_HPP
#define DCT_HPP

// https://unix4lyfe.org/dct-1d/
#include <cmath>

void aan_dct(const double i[], double o[]) {
#if 1
  static const double a1 = sqrt(.5);
  static const double a2 = sqrt(2.) * cos(3./16. * 2 * M_PI);
  static const double a3 = a1;
  static const double a4 = sqrt(2.) * cos(1./16. * 2 * M_PI);
  static const double a5 = cos(3./16. * 2 * M_PI);
#else
  const double a1 = 0.707;
  const double a2 = 0.541;
  const double a3 = 0.707;
  const double a4 = 1.307;
  const double a5 = 0.383;
#endif

  double b0 = i[0] + i[7];
  double b1 = i[1] + i[6];
  double b2 = i[2] + i[5];
  double b3 = i[3] + i[4];
  double b4 =-i[4] + i[3];
  double b5 =-i[5] + i[2];
  double b6 =-i[6] + i[1];
  double b7 =-i[7] + i[0];

  double c0 = b0 + b3;
  double c1 = b1 + b2;
  double c2 =-b2 + b1;
  double c3 =-b3 + b0;
  double c4 =-b4 - b5;
  double c5 = b5 + b6;
  double c6 = b6 + b7;
  double c7 = b7;

  double d0 = c0 + c1;
  double d1 =-c1 + c0;
  double d2 = c2 + c3;
  double d3 = c3;
  double d4 = c4;
  double d5 = c5;
  double d6 = c6;
  double d7 = c7;

  double d8 = (d4 + d6) * a5;

  double e0 = d0;
  double e1 = d1;
  double e2 = d2 * a1;
  double e3 = d3;
  double e4 = -d4 * a2 - d8;
  double e5 = d5 * a3;
  double e6 = d6 * a4 - d8;
  double e7 = d7;

  double f0 = e0;
  double f1 = e1;
  double f2 = e2 + e3;
  double f3 = e3 - e2;
  double f4 = e4;
  double f5 = e5 + e7;
  double f6 = e6;
  double f7 = e7 - e5;

  double g0 = f0;
  double g1 = f1;
  double g2 = f2;
  double g3 = f3;
  double g4 = f4 + f7;
  double g5 = f5 + f6;
  double g6 = -f6 + f5;
  double g7 = f7 - f4;

#if 1
  const double s0 = (cos(0)*sqrt(.5)/2)/(1       );  // 0.353553
  const double s1 = (cos(1.*M_PI/16)/2)/(-a5+a4+1);  // 0.254898
  const double s2 = (cos(2.*M_PI/16)/2)/(a1+1    );  // 0.270598
  const double s3 = (cos(3.*M_PI/16)/2)/(a5+1    );  // 0.300672
  const double s4 = s0;  // (cos(4.*M_PI/16)/2)/(1       );
  const double s5 = (cos(5.*M_PI/16)/2)/(1-a5    );  // 0.449988
  const double s6 = (cos(6.*M_PI/16)/2)/(1-a1    );  // 0.653281
  const double s7 = (cos(7.*M_PI/16)/2)/(a5-a4+1 );  // 1.281458
#else
  const double s0 = 1.;
  const double s1 = 1.;
  const double s2 = 1.;
  const double s3 = 1.;
  const double s4 = 1.;
  const double s5 = 1.;
  const double s6 = 1.;
  const double s7 = 1.;
#endif

  o[0] = g0 * s0;
  o[4] = g1 * s4;
  o[2] = g2 * s2;
  o[6] = g3 * s6;
  o[5] = g4 * s5;
  o[1] = g5 * s1;
  o[7] = g6 * s7;
  o[3] = g7 * s3;
}

#if 0
#include <cstdio>
#include <cstdlib>
#include "dct_a.cc"

void print(const double d[8]) {
  printf("[");
  for (int i = 0; i<8; ++i) {
	printf(" %6.3f", d[i]);
  }
  printf(" ]\n");
}

int main() {
  double a[8];
  for (int i = 0; i<8; ++i) {
	a[i] = 1.;
  }
  double b[8];
  idct(8, a, b);
  printf("take   "); print(a);  // all 1s
  printf("idct = "); print(b);
  double c[8];
  dct_ii(8, b, c);
  printf(" dct = "); print(c);  // all 1s
  double d[8];
  aan_dct(b, d);
  printf(" aan = "); print(d);
  printf("scale factors:\n");
  for (int i = 0; i<8; ++i) {
	printf("const double s%d = %a;  // %f\n", i, a[i] / d[i], a[i] / d[i]);
  }
  // test random inputs.
  for (int j = 0; j<50; ++j) {
	for (int i = 0; i<8; ++i) {
	  a[i] = drand48() * 18. - 9.;
	}
	dct_ii(8, a,b);
	aan_dct(a, c);
	//print(a);
	//print(b);
	//print(c);
	double err = 0;
	for (int i = 0; i<8; ++i) {
	  d[i] = b[i] - c[i];
	  err += d[i] * d[i];
	}
	//print(d);
	printf("err = %f, input = ", err);
	print(a);
  }
}
#endif

#endif // DCT_HPP
