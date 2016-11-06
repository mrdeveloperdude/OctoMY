#ifndef TREMOLO_HPP
#define TREMOLO_HPP

#include "IAudioSource.hpp"

class Tremolo : public IAudioProcessor
{
private:
	QAudioFormat af;
	double freq;
	double amp;
	double angle;
	double step;
	double ampSR;
	double *buf;
	int size;
	int index;
	bool isInited;
public:
	explicit Tremolo();
	virtual ~Tremolo();

	void set(double freq,double amp);

	//IAudioProcessor interface
public:
	void init(QAudioFormat f) Q_DECL_OVERRIDE;
	void process(qint64 num, double *in, double *out) Q_DECL_OVERRIDE;
};

#endif // TREMOLO_HPP
