#ifndef COMPOSITECONDITION_HPP
#define COMPOSITECONDITION_HPP

#include "uptime/SharedPointerWrapper.hpp"
#include "trigger/condition/Condition.hpp"

#include <QList>

class CompositeCondition : public ConditionBase
{
private:
	LogicalOp mOperator;
	QList<QSharedPointer<ICondition> > mChildren;

public:
	static const QString composite_name;
	
public:
	CompositeCondition(LogicalOp op = LogicalOp::AND);
	virtual ~CompositeCondition() = default;
	
	static QString staticName() { return composite_name; }
	QString name() const override{ return staticName(); };
	
	// Adds a child condition.
	void addCondition(const QSharedPointer<ICondition> condition);
	// Returns the list of child conditions.
	const QList<QSharedPointer<ICondition> > &children() const;
	
	// Evaluate by applying the logical operator over children.
	bool actualEvaluate(ConditionContext &conditionContext) override;
	
	// Reorders children based on their last evaluation time (cheaper first).
	void reorderChildren();
	
	LogicalOp logicalOperator() const;
	void setLogicalOperator(LogicalOp logicalOperator);

	QString toHTML(const QString &link_prefix, bool editMode=true) const;
	
	QSharedPointer<ICondition> locate(const QString &id) const;
	void remove(const QString &id) ;

};


#endif // COMPOSITECONDITION_HPP
