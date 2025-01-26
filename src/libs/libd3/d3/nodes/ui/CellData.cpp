#include "CellData.hpp"



void CellData::update() {
	if (expression.startsWith('=')) {
		// TODO: Implement formula parsing and evaluation
		value = 42.0; // Placeholder value
	} else if (expression.isEmpty()) {
		value = 0.0;
	} else {
		value = expression.toDouble(); // Interpret as number if possible
	}
}


