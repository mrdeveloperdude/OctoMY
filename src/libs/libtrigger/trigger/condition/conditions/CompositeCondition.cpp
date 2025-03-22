#include "CompositeCondition.hpp"

#include "trigger/condition/LogicalOp.hpp"
#include "trigger/phrase/ParameterPhrase.hpp"
#include "uptime/SharedPointerWrapper.hpp"

class ICondition;

const QString CompositeCondition::composite_name("Composite");

CompositeCondition::CompositeCondition(LogicalOp op)
	: ConditionBase("This condition is met based on other conditions.")
	, mOperator(op)
{
}

void CompositeCondition::addCondition(const QSharedPointer<ICondition> condition)
{
	mChildren.append(condition);
}


const QList<QSharedPointer<ICondition> > &CompositeCondition::children() const
{
	return mChildren;
}


bool CompositeCondition::actualEvaluate(ConditionContext &conditionContext)
{
	bool result{false};
	switch(mOperator)
	{
	default:
		qWarning() << "unsupported operator" << logicalOpToString(mOperator);
	case LogicalOp::AND:
	{
		result = true;
		for(auto &cond : mChildren)
		{
			if(!cond->evaluate(conditionContext))
			{
				result = false;
				break;
			}
		}
		break;
	}
	case LogicalOp::OR:
	{
		result = false;
		for(auto &cond : mChildren)
		{
			if(cond->evaluate(conditionContext))
			{
				result = true;
				break;
			}
		}
		break;
	}
	case LogicalOp::NOT:
	{
		if(mChildren.size() == 1)
		{
			result = !mChildren.first()->evaluate(conditionContext);
		}
		else
		{
			qWarning() << "NOT operator with multiple arguments";
			result = false;
		}
		break;
	}
	}
	reorderChildren();
	return result;
}


void CompositeCondition::reorderChildren()
{
	std::sort(mChildren.begin(), mChildren.end(), [](const QSharedPointer<ICondition> &a, const QSharedPointer<ICondition> &b) {
		return a->lastEvaluationTime() < b->lastEvaluationTime();
	});
}


QString CompositeCondition::toHTML(const QString &link_prefix, bool editMode) const{
	QString html;
	const auto composite_id=id();
	auto parent_base_prefix{link_prefix + ParameterPhrase::separator + composite_id + ParameterPhrase::separator};
	html += "<table>\n";
	html += " <tr>\n";
	html += "  <td style=\"padding:10px; border-right:2px solid rgba(255,255,255,0.25); vertical-align: middle;\">\n";
	if(editMode){
		html += QString("   <a href=\"%1\" title=\"Choose Logical Operator\">" + logicalOpToString(mOperator) + "</a>\n").arg(parent_base_prefix + "switch-operator");
	}
	else{
		html += QString("   " + logicalOpToString(mOperator) + "\n");
	}
	html += "  </td>\n";
	html += "  <td style=\"padding:10px;\">\n";
	html += "   <table>\n";
	for (const auto &child:mChildren){
		if(child.isNull()){
			qWarning() << "Skipping missing child";
			continue;
		}
		auto name = child->name();
		//qDebug() << "Name" << name;
		html += "   <tr>\n";
		html += "    <td>\n";
		auto child_base_prefix{link_prefix + ParameterPhrase::separator + child->id() + ParameterPhrase::separator};
		if(CompositeCondition::composite_name == name){
			auto composite = qSharedPointerCast<CompositeCondition>(child);
			if (!composite.isNull()) {
				html += composite->toHTML(link_prefix, editMode);
			}
			else {
				qWarning() << "Name-composite-null" << name;
			}
		}
		else {
			if(editMode){
				html += child->phrase()->filledPhrase(child_base_prefix);
			}
			else{
				html += child->phrase()->filledPhrase();
			}
		}
		if(editMode){
			html += QString("     <a href=\"%1\"><img src=\":/icons/app/add.svg\" height=13/></a>\n").arg(parent_base_prefix + "add-condition");
			html += QString("     <a href=\"%1\"><img src=\":/icons/app/arrow_up.svg\" height=13/></a>\n").arg(child_base_prefix + "move-up-condition");
			html += QString("     <a href=\"%1\"><img src=\":/icons/app/arrow_down.svg\" height=13/></a>\n").arg(child_base_prefix + "move-down-condition");
			html += QString("     <a href=\"%1\"><img src=\":/icons/app/remove.svg\" height=13/></a>\n").arg(child_base_prefix + "remove-condition");
		}
		else{
			html += QString("     <a href=\"#\"><img src=\":/icons/app/transparent.svg\" height=13/></a>\n");
		}
		html += "    </td>\n";
		html += "   </tr>\n";
	}
	html += "   <tr>\n";
	html += "    <td>\n";
		if(editMode && mChildren.empty()){
			html += QString("     <a href=\"%1\"><img src=\":/icons/app/add.svg\" height=13/></a>").arg(parent_base_prefix + "add-condition");
		}
		else{
			html += QString("     <a href=\"#\"><img src=\":/icons/app/transparent.svg\" height=13 /></a>\n");
		}
	html += "    </td>\n";
	html += "   </tr>\n";
	html += "   </table>\n";
	html += "  </td>\n";
	html += " </tr>\n";
	html += "</table>\n";

	return html;
}


LogicalOp CompositeCondition::logicalOperator() const{
	return mOperator;
}


void CompositeCondition::setLogicalOperator(LogicalOp logicalOperator){
	mOperator = logicalOperator;
}


QSharedPointer<ICondition> CompositeCondition::locate(const QString &id) const{
	for(const auto &child:mChildren){
		if(child.isNull()){
			qWarning() << "Skipping missing child";
			continue;
		}
		if(id == child->id()){
			return child;
		}
		if(CompositeCondition::composite_name == child->name()){
			auto composite = qSharedPointerCast<CompositeCondition>(child);
			if (composite.isNull()) {
				qWarning() << "Skipping malformed composite child";
				continue;
			}
			else {
				auto child_found = composite->locate(id);
				if(child_found){
					return child_found;
				}
			}
		}
	}
	return nullptr;
}


void CompositeCondition::remove(const QString &id) {
	for(const auto &child:mChildren){
		if(child.isNull()){
			qWarning() << "Skipping missing child";
			continue;
		}
		if(id == child->id()){
			
		}
		if(CompositeCondition::composite_name == child->name()){
			auto composite = qSharedPointerCast<CompositeCondition>(child);
			if (composite.isNull()) {
				qWarning() << "Skipping malformed composite child";
				continue;
			}
			else {
				composite->remove(id);
			}
		}
	}
}

