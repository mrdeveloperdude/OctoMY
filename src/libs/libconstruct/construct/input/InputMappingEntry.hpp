#ifndef INPUTMAPPINGENTRY_HPP
#define INPUTMAPPINGENTRY_HPP

#include "InputState.hpp"
#include <QSet>


enum class InputAction {
	  None
	, Dolly
	, Pan
	, Look
	, Orbit
	, Select
	, MultiSelect
	, ToggleSelection
	, ResetView
};

// This structure encodes the conditions required for an action.
// In InputMappingEntry.hpp:
struct InputMappingEntry {
	// The action to trigger when all conditions are met.
	InputAction action = InputAction::None;
	
	// Conditions: sets of keys that must be pressed or not pressed.
	QSet<int> requiredKeys;
	QSet<int> forbiddenKeys;
	
	// Mouse button conditions.
	bool requireLeftButton   = false;
	bool requireRightButton  = false;
	bool requireMiddleButton = false;
	
	// Modifier key conditions.
	bool requireShift = false;
	bool requireCtrl  = false;
	bool requireAlt   = false;
	bool requireMeta  = false;
	
	// For example, if we need a nonzero scroll delta.
	bool requireScrollNonZero = false;
	
	// Axis multipliers for mouse moves/scroll (facilitates inversion with negative values)
	qreal multiplierXAxis{1.0};
	qreal multiplierYAxis{1.0};
	qreal multiplierWAxis{1.0};
	
	// Flag to indicate if the delta should be computed incrementally (FPS-style)
	// When false, the delta is computed relative to the button-down position (shuttle mode)
	bool fpsDelta = false;
	
	// New flag: if true, capture the mouse so that the drag remains uninterrupted.
	bool captureMouse = false;
	
	bool matches(const InputState &state) const {
		// Check required keys.
		for (int key : requiredKeys) {
			if (!state.pressedKeys.contains(key))
				return false;
		}
		// Check forbidden keys.
		for (int key : forbiddenKeys) {
			if (state.pressedKeys.contains(key))
				return false;
		}
		// Check mouse button conditions.
		if (requireLeftButton && !state.leftButtonDown)
			return false;
		if (requireRightButton && !state.rightButtonDown)
			return false;
		if (requireMiddleButton && !state.middleButtonDown)
			return false;
		// Check modifier keys.
		if (requireShift && !state.shiftDown)
			return false;
		if (requireCtrl && !state.ctrlDown)
			return false;
		if (requireAlt && !state.altDown)
			return false;
		if (requireMeta && !state.metaDown)
			return false;
		// Check scroll condition.
		if (requireScrollNonZero && state.scrollDelta == 0.0)
			return false;
		return true;
	}
};

#endif // INPUTMAPPINGENTRY_HPP
