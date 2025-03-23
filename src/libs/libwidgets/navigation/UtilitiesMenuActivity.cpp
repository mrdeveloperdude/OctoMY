#include "UtilitiesMenuActivity.hpp"

#include "address/Associate.hpp"
#include "hub/HubWindow.hpp"
#include "node/Node.hpp"
#include "node/NodeFactory.hpp"
#include "uptime/MethodGate.hpp"

#include <QDebug>

UtilitiesMenuActivity::UtilitiesMenuActivity(QWidget *parent)
	: MenuActivity(parent, "UtilitiesMenuActivity")
	, mConfigureHelper("UtilitiesMenuActivity", true, false, false, true, false)
	
{
	OC_METHODGATE();
}


UtilitiesMenuActivity::~UtilitiesMenuActivity()
{
	OC_METHODGATE();
}


void UtilitiesMenuActivity::configure(QSharedPointer<Node> n)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		auto hubWindow = qSharedPointerCast<HubWindow>(n->nodeWindow());
		//auto hw = hubWindow.data();
		////////////////////////////////////////////////////////////////////////
		addGroup(tr("Utilities"));
		
		addButton(tr("Nodes Management"),   ":/icons/nodes/nodes.svg",                 tr("Manage Nodes"),                                     "NodesManagementActivity");
		addButton(tr("Triggers"),           ":/icons/trigger/condition.svg",           tr("Work with triggers"),                               "TriggerManagerActivity");
		addButton(tr("Gait"),               ":/icons/controller/gait_control.svg",     tr("Work with gaits and leg motion"),                   "GaitActivity");
		addButton(tr("Map"),                ":/icons/map/map.svg",                     tr("Work with map"),                                    "MapActivity");
		addButton(tr("Plan"),               ":/icons/plan/mandate.svg",                tr("Work with plans"),                                  "PlanActivity");
		addButton(tr("Editor"),             ":/icons/plan/markdown.svg",               tr("Work with markdown text"),                          "MarkdownActivity");
		addButton(tr("Voice"),              ":/icons/controller/speech.svg",           tr("Work with voice and speech"),                       "VoiceActivity");
		addButton(tr("HUD"),                ":/icons/controller/hud.svg",              tr("Work with heads-up display (HUD)"),                 "HUDActivity");
		addButton(tr("Log"),                ":/icons/general/log.svg",                 tr("Inspect logs"),                                     "LogActivity");
		addButton(tr("Actuator"),           ":/icons/controller/actuator_control.svg", tr("Work with actuators"),                              "ActuatorActivity");
		addButton(tr("Splitter"),           ":/icons/general/splitter.svg",            tr("Work with splitter"),                               "SplitterActivity");


		addSpacer();
	}
}
