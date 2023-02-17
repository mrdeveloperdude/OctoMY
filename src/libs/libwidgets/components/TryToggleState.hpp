#ifndef TRYTOGGLESTATE_HPP
#define TRYTOGGLESTATE_HPP

#include <QtGlobal>

enum TryToggleState: quint8 {
	OFF,GOING_ON,ON, GOING_OFF
};


class QString;
class QDebug;

QString ToggleStateToSTring(TryToggleState s);



QDebug operator<<(QDebug d, const TryToggleState &s);

bool positive(TryToggleState s);
bool transient(TryToggleState s);

TryToggleState saturate(TryToggleState s);
TryToggleState desaturate(TryToggleState s);

#endif
// TRYTOGGLESTATE_HPP
