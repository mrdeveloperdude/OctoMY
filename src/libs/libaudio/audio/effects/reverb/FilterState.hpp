#ifndef FILTERSTATE_HPP
#define FILTERSTATE_HPP

#include "uptime/MethodGate.hpp"

#include <QtMath>


enum FilterType {
	// EFX-style low-pass filter, specifying a gain and reference frequency.
	Filter_HighShelf,
	// EFX-style high-pass filter, specifying a gain and reference frequency.
	Filter_LowShelf,
};


class FilterState
{
public:
	qreal x[2]; // History of two last input samples
	qreal y[2]; // History of two last output samples
	qreal a[3]; // Transfer function coefficients "a"
	qreal b[3]; // Transfer function coefficients "b"

public:

	FilterState()
		: x{0,0}
		, y{0,0}
		, a{0,0,0}
		, b{0,0,0}
	{

	}

public:

	void clear()
	{
		x[0] = 0.0f;
		x[1] = 0.0f;
		y[0] = 0.0f;
		y[1] = 0.0f;
	}
public:

	inline qreal process(qreal sample)
	{
		OC_METHODGATE();
		qreal outsmp;

		outsmp = b[0] * sample +
				 b[1] * x[0] +
				 b[2] * x[1] -
				 a[1] * y[0] -
				 a[2] * y[1];
		x[1] = x[0];
		x[0] = sample;
		y[1] = y[0];
		y[0] = outsmp;

		return outsmp;
	}


	void setParams(FilterType type, qreal gain, qreal freq_mult, qreal bandwidth)
	{
		OC_METHODGATE();
		Q_UNUSED(bandwidth);
		qreal alpha;
		qreal w0;

		// Limit gain to -100dB
		gain = std::max(gain, 0.00001);

		w0 = M_2_PI * freq_mult;

		// Calculate filter coefficients depending on filter type
		switch(type) {
		case Filter_HighShelf:
			alpha = sinf(w0)/2.0f*sqrtf((gain + 1.0f/gain)*(1.0f/0.75f - 1.0f) + 2.0f);
			b[0] =       gain*((gain+1.0f) + (gain-1.0f)*cosf(w0) + 2.0f*sqrtf(gain)*alpha);
			b[1] = -2.0f*gain*((gain-1.0f) + (gain+1.0f)*cosf(w0)                         );
			b[2] =       gain*((gain+1.0f) + (gain-1.0f)*cosf(w0) - 2.0f*sqrtf(gain)*alpha);
			a[0] =             (gain+1.0f) - (gain-1.0f)*cosf(w0) + 2.0f*sqrtf(gain)*alpha;
			a[1] =  2.0f*     ((gain-1.0f) - (gain+1.0f)*cosf(w0)                         );
			a[2] =             (gain+1.0f) - (gain-1.0f)*cosf(w0) - 2.0f*sqrtf(gain)*alpha;
			break;
		case Filter_LowShelf:
			alpha = sinf(w0)/2.0f*sqrtf((gain + 1.0f/gain)*(1.0f/0.75f - 1.0f) + 2.0f);
			b[0] =       gain*((gain+1.0f) - (gain-1.0f)*cosf(w0) + 2.0f*sqrtf(gain)*alpha);
			b[1] =  2.0f*gain*((gain-1.0f) - (gain+1.0f)*cosf(w0)                         );
			b[2] =       gain*((gain+1.0f) - (gain-1.0f)*cosf(w0) - 2.0f*sqrtf(gain)*alpha);
			a[0] =             (gain+1.0f) + (gain-1.0f)*cosf(w0) + 2.0f*sqrtf(gain)*alpha;
			a[1] = -2.0f*     ((gain-1.0f) + (gain+1.0f)*cosf(w0)                         );
			a[2] =             (gain+1.0f) + (gain-1.0f)*cosf(w0) - 2.0f*sqrtf(gain)*alpha;
			break;
		}

		b[2] /= a[0];
		b[1] /= a[0];
		b[0] /= a[0];
		a[2] /= a[0];
		a[1] /= a[0];
		a[0] /= a[0];
	}

};

#endif // FILTERSTATE_HPP
