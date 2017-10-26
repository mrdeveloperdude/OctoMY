#ifndef TRYTOGGLESTATE_HPP
#define TRYTOGGLESTATE_HPP


enum TryToggleState {
	OFF,GOING_ON,ON, GOING_OFF
};


class QString;
class QDebug;

QString ToggleStateToSTring(TryToggleState s);



QDebug operator<<(QDebug d, const TryToggleState &s);


#endif // TRYTOGGLESTATE_HPP
