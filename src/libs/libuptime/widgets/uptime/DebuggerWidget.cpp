#include "DebuggerWidget.hpp"
#include "ui_DebuggerWidget.h"

#include "node/NodeWindow.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/ConnectionType.hpp"
#include "utility/time/HumanTime.hpp"

#include "service/ServiceLevelManager.hpp"
#include "service/ServiceManager.hpp"

#include "name/AgentNameGenerator.hpp"
#include "name/GenderGenerator.hpp"

#include <QTableWidgetItem>
#include <QWidget>


DebuggerWidget::DebuggerWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::Debugger)
	, mConfigureHelper("DebuggerWidget", true, false, false, true, false)
{
	OC_METHODGATE();
	ui->setupUi(this);
	setEnabled(false);
	qRegisterMetaType<QSharedPointer <NodeWindow>> ("QSharedPointer <NodeWindow>");
	qRegisterMetaType<QSharedPointer <DebuggerWidget>> ("QSharedPointer <DebuggerWidget>");
	Q_INIT_RESOURCE(debug_icons);
}


DebuggerWidget::~DebuggerWidget()
{
	OC_METHODGATE();
	delete ui;
}


static void configTryToggleForServiceLevel(TryToggle *tt, const QString serviceLevel, const QSharedPointer<Node> node, const QString offText, const QString goingOnText, const QString onText, const QString goingOffText)
{
	OC_FUNCTIONGATE();
	if(nullptr != tt) {
		if(!node.isNull()) {
			tt->configure(offText, goingOnText, onText, goingOffText);
			if(!QObject::connect(tt, &TryToggle::stateChanged, node.data(), [=](const TryToggleState last, const TryToggleState current) {
			Q_UNUSED(last);
				auto slm=node->serviceLevelManager();
				if(!slm.isNull()) {
					slm->enableLevel(serviceLevel, positive(current), [=](bool ok) {
						auto res=saturate(ok?current:last);
						qDebug()<<"TRY TOGGLE '"<<serviceLevel<<"' OK: "<<ok<<" RESULTED IN: "<<res;
						tt->setState(res, false);
					});
				} else {
					qWarning()<<"WARNING: Could not switch discovery service, no service manager";
				}
			}, OC_CONTYPE)) {
			}
		} else {
			qWarning()<<"WARNING: Could not switch service, no node";
		}
	} else {
		qWarning()<<"WARNING: No try toggle in configTryToggleForServiceLevel()";
	}
}


void DebuggerWidget::configure(QSharedPointer <Node> node)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mNode=node;
		setEnabled(!mNode.isNull());
		if(!mNode.isNull()) {
			if(!connect(mNode.data(), &Node::identityChanged, this, [this]() {
			updateIdentity();
			},OC_CONTYPE)) {
				qWarning()<<"ERROR: Could not connect";
			}
		}
		// Not necessary
		//ui->widgetIdenticon->configure();
		ui->widgetBirthCertificate->configure();
		updateIdentity();
		ui->widgetHeaderCommsChannel->configure(mNode);
		ui->widgetHeaderCommsCarrier->configure(mNode);
		ui->widgetHeaderServices->configure(mNode);
		ui->widgetHeaderKeyStore->configure(mNode);
		ui->widgetHeaderLocalIdentity->configure(mNode);
		ui->widgetHeaderPairing->configure(mNode);
		ui->widgetHeaderLocalAddresses->configure(mNode);
		ui->widgetNetworkSettings->configure(mNode?mNode->localAddressList():nullptr);
		ui->widgetHeaderDiscoveryClient->configure(mNode);
		configTryToggleForServiceLevel(ui->tryToggleDiscovery, "Discovery", mNode, "Activate disovery", "Discovery activating", "Deactivate discovery", "Discovery deactivating");
		configTryToggleForServiceLevel(ui->tryToggleAlways, "Always", mNode, "Activate", "Activating", "Deactivate", "Deactivating");
		if(nullptr != ui->tryToggleBirth) {
			if(!node.isNull()) {
				ui->tryToggleBirth->configure("Birth", "Delivering", "Unbirth", "Killing");
				if(!QObject::connect(ui->tryToggleBirth, &TryToggle::stateChanged, mNode.data(), [=](const TryToggleState last, const TryToggleState current) {
				Q_UNUSED(last);
					if(transient(current)) {
						positive(current)?birth():unBirth();
					}
				}, OC_CONTYPE)) {
				}
			} else {
				qWarning()<<"WARNING: Could not switch birth, no node";
			}
		} else {
			qWarning()<<"WARNING: No try birth try toggle";
		}
		configureUi();
		updateExpandButton();
	}
}


void DebuggerWidget::updateServiceTable()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		ui->widgetHeaderServices->updateServiceTable();
	}
}


void DebuggerWidget::updateIdentity()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(!mNode.isNull()) {
			auto identity=mNode->nodeIdentity();
			if(!identity.isNull()) {
				auto pid=identity->toPortableID();
				qDebug()<<"IDENTITY "<<pid.toPortableString();
				ui->widgetIdenticon->setPortableID(pid);
				ui->widgetBirthCertificate->setPortableID(pid);
				ui->tryToggleBirth->setState(pid.id().isEmpty()?TryToggleState::OFF:TryToggleState::ON, false);
			}
		}
	}
}


void DebuggerWidget::birth()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(!mNode.isNull()) {
			QSharedPointer<KeyStore> keystore=mNode->keyStore();
			if(!keystore.isNull()) {
				keystore->clear([=](ASEvent<QVariantMap> &ase1) {
					Q_UNUSED(ase1);
					keystore->setBootstrapEnabled(true);
					keystore->synchronize([=](ASEvent<QVariantMap> &ase2) {
						Q_UNUSED(ase2);
						QSharedPointer<Key> key=keystore->localKey();
						if(!key.isNull()) {
							QVariantMap map;
							const quint64 now=utility::time::currentMsecsSinceEpoch<quint64>();
							AgentNameGenerator ang;
							GenderGenerator gg;
							map["id"]=key->id();
							map["name"]=ang.generate();
							map["gender"]=gg.generate();
							map["role"]=nodeRoleToString(mNode->nodeRole());
							map["type"]=nodeTypeToString(mNode->nodeType());
							map["birthDate"]=utility::time::msToVariant(now);
							qDebug()<<"Creating new identity: "<<map;
							mNode->setNodeIdentity(map);
						}
					});
				});
			}
		} else {
			qWarning()<<"WARNING: Could not birth, no node";
		}
	}
}


void DebuggerWidget::unBirth()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(!mNode.isNull()) {
			mNode->unbirth();
		} else {
			qWarning()<<"WARNING: Could not unbirth, no node";
		}
	}
}


void DebuggerWidget::configureUi()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		QList<QWidget *> allWidgets= this->findChildren<QWidget *>();
		const QString widgetBaseName("widgetHeader");
		const auto widgetBaseNameSize=widgetBaseName.size();
		for(QWidget *widget: allWidgets) {
			if((nullptr!= widget) && (widget->objectName().startsWith(widgetBaseName))) {
				QString name=widget->objectName().remove(0, widgetBaseNameSize);
				mHeaderWidgets[name]=widget;
				widget->setVisible(false);
				// qDebug().noquote().nospace()<<"Preparing widget for "<<name;
			}
		}
		QList<QPushButton *> allButtons= this->findChildren<QPushButton *>();
		const QString buttonBaseName("pushButtonHeader");
		const auto buttonBaseNameSize=buttonBaseName.size();
		for(QPushButton *button: allButtons) {
			if((nullptr!= button) && (button->objectName().startsWith(buttonBaseName))) {
				QString name=button->objectName().remove(0, buttonBaseNameSize);
				// qDebug().noquote().nospace()<<"Connecting widget to button for "<<name;
				mHeaderButtons+=button;
				button->setCheckable(true);
				button->setChecked(false);
				if(!connect(button, &QPushButton::toggled, this, [=](bool checked) {
				QWidget *widget=mHeaderWidgets.contains(name)?mHeaderWidgets[name]:nullptr;
					if(nullptr!=widget) {
						qDebug()<<"TOGGLING BUTTON "<<widget->objectName()<<" for "<<name;
						widget->setVisible(checked);
						pack();
						updateExpandButton();
					}
				}, OC_CONTYPE)) {
					qWarning()<<"ERROR: Could not connect";
				}
			}
		}
		Qt::WindowFlags flags = nullptr;
//flags |= Qt::MSWindowsFixedSizeDialogHint;
//flags |= Qt::X11BypassWindowManagerHint;
//flags |= Qt::FramelessWindowHint;
		flags |= Qt::WindowTitleHint;
//flags |= Qt::WindowSystemMenuHint;
		flags |= Qt::WindowMinimizeButtonHint;
//flags |= Qt::WindowMaximizeButtonHint;
//flags |= Qt::WindowCloseButtonHint;
//flags |= Qt::WindowContextHelpButtonHint;
//flags |= Qt::WindowShadeButtonHint;
		flags |= Qt::WindowStaysOnTopHint;
//flags |= Qt::WindowStaysOnBottomHint;
//flags |= Qt::CustomizeWindowHint;
		setWindowFlags(flags);
		pack();
		tuck();
	}
}


static void placeRelative(QWidget *a, QWidget *b, QPoint p, bool right=true)
{
	OC_FUNCTIONGATE();
	if(nullptr==a) {
		return;
	}
	if(nullptr==b) {
		return;
	}
	QRect geoA=a->geometry();
	QRect geoB=b->geometry();
	QRect geoNew(geoB);
	right?geoNew.moveTopLeft(geoA.topRight()+p):geoNew.moveTopRight(geoA.topLeft()-p);
	if(geoB != geoNew) {
		QTimer::singleShot(0, [b, geoNew]() {
			//const auto old=b->blockSignals(true);
			b->setGeometry(geoNew);
			//b->blockSignals(old);
		});
	}
}


void DebuggerWidget::tuck(bool attach, QWidget *movedWindow)
{
	OC_METHODGATE();
	// qDebug()<<"TUCK CALLED WITH :" << attach<< movedWindow;
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(!mNode.isNull()) {
			//Place us to the right of main window
			auto nodeWindow=mNode->nodeWindow();
			if(!nodeWindow.isNull()) {
				if(nullptr==movedWindow){
					movedWindow=this;
				}
				if(movedWindow==this) {
					placeRelative(this, nodeWindow.data(), attach?QPoint(5,0):QPoint(20,((qrand()%30)-15)), false);
				} else {
					// TODO: Figure out how to circumvent the buggy movement when this is enabled
					// placeRelative(nodeWindow.data(), this, attach?QPoint(5,0):QPoint(20,((qrand()%30)-15)), true);
				}
			} else {
				qWarning()<<"ERROR: No node window";
			}
		} else {
			qWarning()<<"ERROR: No node";
		}
	}
}


void DebuggerWidget::pack()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		QTimer::singleShot(0, this, [this]() {
			// Calculate minimum size
			int h=ui->horizontalLayoutTop->minimumSize().height();
			int w=0;
			const int layoutSpacing = ui->horizontalLayoutTop->spacing();
			for(auto widget:mHeaderWidgets) {
				if(widget->isVisible()) {
					auto sz=widget->minimumSize();
					//qDebug()<<"HEADER WIDGET HEIGHT: "<<sz.height()<<" FOR "<<widget->objectName();
					w=qMax(w, sz.width());
					h+=sz.height()+layoutSpacing;
				}
			}
			for(auto widget:mHeaderButtons) {
				if(widget->isVisible()) {
					auto sz=widget->minimumSize();
					//qDebug()<<"HEADER BUTTON HEIGHT: "<<sz.height()<<" FOR "<<widget->objectName();
					w=qMax(w, sz.width());
					h+=sz.height()+layoutSpacing;
				}
			}
			// Apply minimum size
			//qDebug()<<"TOTAL: "<<w<<"x"<<h;
			resize(QSize(w,h));
		});
	}
}


void DebuggerWidget::showEvent(QShowEvent *event)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		Q_UNUSED(event);
		emit visibilityChanged(true);
	}
}


void DebuggerWidget::hideEvent(QHideEvent *event)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		Q_UNUSED(event);
		emit visibilityChanged(false);
	}
}


void DebuggerWidget::on_pushButtonQuitSuccess_clicked()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(!mNode.isNull()) {
			emit mNode->nodeRequestExit(EXIT_SUCCESS);
		} else {
			qWarning()<<"WARNING: Could not emit successfull quit, no node";
		}
	}
}


void DebuggerWidget::on_pushButtonQuitFail_clicked()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(!mNode.isNull()) {
			emit mNode->nodeRequestExit(EXIT_FAILURE);
		} else {
			qWarning()<<"WARNING: Could not emit failure quit, no node";
		}
	}
}


void DebuggerWidget::moveEvent(QMoveEvent *event)
{
	OC_METHODGATE();
	if(nullptr!= event) {
		emit debuggerWindowMoved(sharedFromThis());
	}
}


void DebuggerWidget::on_pushButtonTuckWindow_toggled(bool checked)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		// Make a gesture to show the effect (UX)
		tuck(checked);
		if(!mNode.isNull()) {
			//Place us to the right of main window
			auto nodeWindow=mNode->nodeWindow();
			if(!nodeWindow.isNull()) {
				if(checked) {
					mTuckNodeWindowConnection=connect(nodeWindow.data(), &NodeWindow::nodeWindowMoved, this, [=](QSharedPointer <NodeWindow> nodeWindow) {
						tuck(true, nodeWindow.data());
					}, OC_CONTYPE);
					if(!mTuckNodeWindowConnection) {
						qWarning()<<"ERROR: Could not connect";
					}
					mTuckDebuggerWindowConnection=connect(this, &DebuggerWidget::debuggerWindowMoved, this, [=](QSharedPointer <DebuggerWidget> debuggerWindow) {
						tuck(true, debuggerWindow.data());
					}, OC_CONTYPE);
					if(!mTuckDebuggerWindowConnection) {
						qWarning()<<"ERROR: Could not connect";
					}
				} else {
					if(mTuckNodeWindowConnection && !disconnect(mTuckNodeWindowConnection)) {
						qWarning()<<"ERROR: Could not disconnect";
					}
					if(mTuckDebuggerWindowConnection && !disconnect(mTuckDebuggerWindowConnection)) {
						qWarning()<<"ERROR: Could not disconnect";
					}
				}
			} else {
				qWarning()<<"ERROR: No node window";
			}
		} else {
			qWarning()<<"ERROR: No node";
		}
	}
}


void DebuggerWidget::on_pushButtonRestart_clicked()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(!mNode.isNull()) {
			emit mNode->nodeRequestRestart();
		} else {
			qWarning()<<"WARNING: Could not emit restart, no node";
		}
	}
}


void DebuggerWidget::allEnable(bool enable)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		for(auto button: mHeaderButtons) {
			if(nullptr!=button) {
				auto old=button->blockSignals(true);
				button->setChecked(enable);
				button->blockSignals(old);
			}
		}
		for(auto widget: mHeaderWidgets) {
			if(nullptr!=widget) {
				auto old=widget->blockSignals(true);
				widget->setVisible(enable);
				widget->blockSignals(old);
			}
		}
		pack();
		updateExpandButton();
	}
}


EnabledCount DebuggerWidget::numEnabled()
{
	OC_METHODGATE();
	EnabledCount count;
	if(mConfigureHelper.isConfiguredAsExpected()) {
		for(auto widget: mHeaderWidgets) {
			if(nullptr!=widget) {
				count.count(widget->isVisible());
			}
		}
	}
	return count;
}


void DebuggerWidget::updateExpandButton()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		const EnabledCount count=numEnabled();
		const bool allOn=count.allIs(true);
		QIcon icon;
		icon.addFile(allOn?QStringLiteral(":/icons/collapse_all.svg"):QStringLiteral(":/icons/expand_all.svg"), QSize(), QIcon::Normal, QIcon::Off);
		ui->pushButtonExpandOrCollapseAll->setIcon(icon);
		ui->pushButtonExpandOrCollapseAll->setToolTip(allOn?"Collapse all":"Expand all");
	}
}


void DebuggerWidget::on_pushButtonExpandOrCollapseAll_clicked()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		const EnabledCount count=numEnabled();
		const bool allOn=count.allIs(true);
		allEnable(!allOn);
	}
}
