#ifndef DEVELOPERWIDGET_HPP
#define DEVELOPERWIDGET_HPP

#include <QWidget>
#include <QDir>

#include "app/Settings.hpp"
#include "components/TryToggleState.hpp"
#include "node/NodeFactory.hpp"
#include "uptime/ConfigureHelper.hpp"
#include "uptime/SharedPointerWrapper.hpp"

class Hub;
class IdenticonWidget;
class IrisWidget;
class LogDevice;


template<typename T>
class GaitController;

namespace Ui {
class DeveloperWidget;
}

class DeveloperWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::DeveloperWidget *ui;
	GaitController<qreal> *mGait;
	LogDevice *mLogDevice{nullptr};
	QSharedPointer<Hub> mHub;
	NodeFactory mNodeFactory;
	const QDir mTopDir;
	QDir mWidgetIllustrationOutputDir;
	bool mDebug{false};
	ConfigureHelper mConfigureHelper;
	
public:
	explicit DeveloperWidget(QWidget *parent = nullptr);
	~DeveloperWidget();

private:
	
	QSharedPointer<Hub> hub();
	
private slots:
	void onListenStateChanged(const TryToggleState last, const TryToggleState current);
	void generateTriggers();
	void startConstilation();

public:
	void configure(QSharedPointer<Settings>  settings, QSharedPointer<Hub> hub=nullptr);
	void appendLog(const QString& text);
	void updateIdentityWidgets();


};

#endif // DEVELOPERWIDGET_HPP
