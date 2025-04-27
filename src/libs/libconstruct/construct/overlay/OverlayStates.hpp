#ifndef OVERLAYSTATES_HPP
#define OVERLAYSTATES_HPP

#include <QFlags>

// Define overlay state flags.
enum class OverlayStateFlag {
	None         = 0x00,
	Selected     = 0x01,
	Disabled     = 0x02,
	Hovered      = 0x04,
	ActiveHandle = 0x08
};
Q_DECLARE_FLAGS(OverlayStates, OverlayStateFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(OverlayStates)


#endif // OVERLAYSTATES_HPP
