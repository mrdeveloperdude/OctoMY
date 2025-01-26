#ifndef CELLDATA_HPP
#define CELLDATA_HPP

#include <QString>

class CellData {
public:
	QString expression;  // Stores the expression, e.g., "=A1+B1" or raw text
	qreal value{0.0};    // Stores the computed value
	
	void update();       // Update `value` based on `expression`
	
	// Optional: Extendable in the future
};


#endif // CELLDATA_HPP
