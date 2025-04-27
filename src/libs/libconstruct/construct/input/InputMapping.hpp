#ifndef INPUTMAPPING_HPP
#define INPUTMAPPING_HPP

#include "InputMappingEntry.hpp"
#include <vector>

struct InputMapping {
	// A collection of data-driven mapping entries.
	std::vector<InputMappingEntry> entries;
	InputMappingEntry none;
	qreal panSensitivity{1};
	qreal dollySensitivity{1};
	qreal rotateSensitivity{1};
	qreal orbitSensitivity{1};
	
	// Add a new mapping entry.
	void addMapping(const InputMappingEntry &entry) {
		entries.push_back(entry);
	}
	
	// Evaluate the mappings in order and return the first action that matches.
	const InputMappingEntry &evaluate(const InputState &state) const {
		for (const auto &entry : entries) {
			if (entry.matches(state))
				return entry;
		}
		return none;
	}
	
	
	void defaultMapping(){
		{
			InputMappingEntry entry;
			entry.action = InputAction::Dolly;
			entry.requireScrollNonZero = true;
			entry.multiplierWAxis = 0.002;
			entry.fpsDelta = true;
			addMapping(entry);
		}
		
		{
			InputMappingEntry entry;
			entry.action = InputAction::Pan;
			entry.requireLeftButton = true;
			entry.captureMouse = true;
			entry.multiplierXAxis = -0.02;
			entry.multiplierYAxis = +0.02;
			entry.fpsDelta = true;
			addMapping(entry);
		}
		
		{
			InputMappingEntry entry;
			entry.action = InputAction::Look;
			entry.requireRightButton = true;
			entry.captureMouse = true;
			entry.multiplierXAxis = -0.1;
			entry.multiplierYAxis = -0.1;
			entry.fpsDelta = true;
			addMapping(entry);
		}
		{
			InputMappingEntry entry;
			entry.action = InputAction::Orbit;
			entry.requireMiddleButton = true;
			entry.captureMouse = true;
			entry.multiplierXAxis = -0.1;
			entry.multiplierYAxis = -0.1;
			entry.fpsDelta = false;
			addMapping(entry);
		}
		{
			InputMappingEntry entry;
			entry.action = InputAction::Select;
			entry.requireLeftButton = true;
			entry.forbiddenKeys.insert(Qt::ControlModifier);
			entry.forbiddenKeys.insert(Qt::ShiftModifier);
			addMapping(entry);
		}
		
		{
			InputMappingEntry entry;
			entry.action = InputAction::MultiSelect;
			entry.requireLeftButton = true;
			entry.requireShift = true;
			addMapping(entry);
		}
		
		{
			InputMappingEntry entry;
			entry.action = InputAction::ToggleSelection;
			entry.requireLeftButton = true;
			entry.requireCtrl = true;
			addMapping(entry);
		}
		{
			InputMappingEntry entry;
			entry.action = InputAction::ResetView;
			entry.requiredKeys.insert(Qt::Key_R);
			addMapping(entry);
		}
		
	}
	
	
	
	void altMapping(){
		{
			// Dolly: if there is any scroll delta.
			InputMappingEntry entry;
			entry.action = InputAction::Dolly;
			entry.requireScrollNonZero = true;
			entry.multiplierWAxis = 0.002;
			addMapping(entry);
		}
		
		{
			// Pan: right mouse button with Shift pressed.
			InputMappingEntry entry;
			entry.action = InputAction::Pan;
			entry.requireRightButton = true;
			entry.requireShift = true;
			entry.multiplierXAxis = -0.001;
			entry.multiplierYAxis = -0.001;
			addMapping(entry);
		}
		
		{
			// Rotate: middle mouse button without modifiers.
			InputMappingEntry entry;
			entry.action = InputAction::Look;
			entry.requireMiddleButton = true;
			entry.multiplierXAxis = -0.01;
			entry.multiplierYAxis = -0.01;
			addMapping(entry);
		}
		
		{
			// Single selection: left mouse button, no Ctrl or Shift.
			InputMappingEntry entry;
			entry.action = InputAction::Select;
			entry.requireLeftButton = true;
			entry.forbiddenKeys.insert(Qt::ControlModifier);
			entry.forbiddenKeys.insert(Qt::ShiftModifier);
			addMapping(entry);
		}
		
		{
			// Multi-select (box selection): left mouse with Shift.
			InputMappingEntry entry;
			entry.action = InputAction::MultiSelect;
			entry.requireLeftButton = true;
			entry.requireShift = true;
			addMapping(entry);
		}
		
		{
			// Toggle selection: left mouse with Ctrl.
			InputMappingEntry entry;
			entry.action = InputAction::ToggleSelection;
			entry.requireLeftButton = true;
			entry.requireCtrl = true;
			addMapping(entry);
		}
		
	}
};




#endif // INPUTMAPPING_HPP
