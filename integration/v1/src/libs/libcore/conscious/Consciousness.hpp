#ifndef CONSCIOUSNESS_HPP
#define CONSCIOUSNESS_HPP

#include <QObject>

class Impression
{

private:
	quint64 mCreateTime;


};

/*!
 * \brief The Consciousness class is best documented
 * <a href="https://en.wikipedia.org/wiki/Consciousness">here</a>.
 */

class Consciousness
{
private:

public:
	explicit Consciousness();
	virtual ~Consciousness() {}

public:

	// When an impression occurs, it is received here
	void ingestImpression(const Impression &);

	// Main loop
	void run()
	{

	}


};

#endif // CONSCIOUSNESS_HPP
