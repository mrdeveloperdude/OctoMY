#include "LogicalOp.hpp"


LogicalOp cycle(LogicalOp op){
	return static_cast<LogicalOp>(
		(static_cast<std::underlying_type_t<LogicalOp>>(op) + 1) %
		static_cast<std::underlying_type_t<LogicalOp>>(LogicalOp::OP_COUNT)
		);
}


QString logicalOpToString(LogicalOp op)
{
	switch(op)
	{
	case LogicalOp::AND:
		return QStringLiteral("AND");
	case LogicalOp::OR:
		return QStringLiteral("OR");
	case LogicalOp::NOT:
		return QStringLiteral("NOT");
	default:
		return QStringLiteral("UNKNOWN");
	}
}
