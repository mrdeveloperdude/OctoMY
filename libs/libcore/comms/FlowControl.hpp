#ifndef FLOWCONTROL_HPP
#define FLOWCONTROL_HPP

#include <QString>

class FlowControl
{
public:
	enum Mode {
		Good,
		Bad
	};
private:

	Mode mMode;
	quint64 mPenaltyTime;
	quint64 mGoodConditionsTime;
	quint64 mPenaltyReductionAccumulator;
public:
	explicit FlowControl();
	virtual ~FlowControl();

public:
	void reset();
	void update( quint64 deltaTime, quint64 rtt );
	quint64 sendRate() const ;

	QString toString(QString sep="\n")const ;

};


#endif // FLOWCONTROL_HPP
