#ifndef LOGICALOP_HPP
#define LOGICALOP_HPP

#include <QString>

// Logical operators for composite conditions.
enum class LogicalOp:quint8
{
	  AND
	, OR
	, NOT
	, OP_COUNT
};


LogicalOp cycle(LogicalOp op);

QString logicalOpToString(LogicalOp op);


#endif // LOGICALOP_HPP
