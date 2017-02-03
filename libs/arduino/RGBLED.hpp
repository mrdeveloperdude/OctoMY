#ifndef RGBLED_HPP
#define RGBLED_HPP



#include "ArduMY.hpp"


struct RGBLEDPins {
	uint8_t pinR;
	uint8_t pinG;
	uint8_t pinB;

	RGBLEDPins(
		uint8_t pinR
		, uint8_t pinG
		, uint8_t pinB
	)
		: pinR(pinR)
		, pinG(pinG)
		, pinB(pinB)
	{
	}
};

struct RGBLED {
	RGBLEDPins pins;
	float hue;
	float intensity;

private:

	static const float amp[3];

	static inline float  hue2rgb(float  p, float  q, float  t)
	{
		if (t < 0) {
			t += 1;
		}
		if (t > 1) {
			t -= 1;
		}
		if (t < 1 / 6.0) {
			return p + (q - p) * 6 * t;
		}
		if (t < 1 / 2.0) {
			return q;
		}
		if (t < 2 / 3.0) {
			return p + (q - p) * (2 / 3.0 - t) * 6;
		}
		return p;
	}

	static inline void hslToRgb(float h, float s, float l, uint8_t rgb[])
	{
		float r, g, b;
		if (s <= 0) {
			r = g = b = l; // achromatic
		} else {
			float q = l < 0.5 ? l * (1 + s) : l + s - l * s;
			float p = 2 * l - q;
			r = hue2rgb(p, q, h + 1 / 3.0);
			g = hue2rgb(p, q, h);
			b = hue2rgb(p, q, h - 1 / 3.0);
		}
		rgb[0] = (amp[0] * r) * 0xFF;
		rgb[1] = (amp[1] * g) * 0xFF;
		rgb[2] = (amp[2] * b) * 0xFF;
	}

public:

	RGBLED( RGBLEDPins &pins, float hue = 0.0, float intensity = 0.5 )
		: pins( pins )
		, hue( hue )
		, intensity ( intensity )
	{

	}

	void setup()
	{

	}

	uint32_t lastTime = 0;

	void update(uint32_t unow)
	{
		uint8_t rgb[3] = {0, 0, 0};
		hslToRgb(hue, 1.0, intensity, rgb);
		analogWrite(pins.pinR, rgb[0]);
		analogWrite(pins.pinG, rgb[1]);
		analogWrite(pins.pinB, rgb[2]);
	}

};


#endif // RGBLED_HPP
