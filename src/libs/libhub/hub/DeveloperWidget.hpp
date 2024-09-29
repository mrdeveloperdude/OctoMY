#ifndef DEVELOPERWIDGET_HPP
#define DEVELOPERWIDGET_HPP

#include <QWidget>
#include <QDir>

#include "app/Settings.hpp"
#include "components/TryToggleState.hpp"

#include "node/NodeFactory.hpp"

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
	const QDir mTopDir;
	QDir mLastDir;
	QDir mWidgetIllustrationOutputDir;
	LogDevice *mLogDevice{nullptr};
	QSharedPointer<Hub> mHub;
	NodeFactory mNodeFactory;
	
public:
	explicit DeveloperWidget(QWidget *parent = nullptr);
	~DeveloperWidget();
	
	
private:
	
	QSharedPointer<Hub> hub();
	
private slots:
	void onListenStateChanged(const TryToggleState last, const TryToggleState current);
	
	void on_pushButtonRandomIdenticonID_clicked();
	
	void on_pushButtonSaveIdenticonAgent_clicked();
	void on_horizontalSliderIrisIndex_valueChanged(int value);
	void on_pushButtonSaveIdenticonIris_clicked();
	void on_pushButtonSaveIdenticonRemote_clicked();
	void on_pushButtonSaveIdenticonHub_clicked();
	void on_pushButtonWidgetIllustrationBrowse_clicked();
	void on_pushButtonWidgetIllustrationSaveAll_clicked();
	
	void generateTriggers();
	
	
	void startConstilation();
	
public:
	void configure(QSharedPointer<Settings>  settings, QSharedPointer<Hub> hub=nullptr);
	void appendLog(const QString& text);
	QString saveIdenticonWidget(IdenticonWidget *iw, QString base="identicon");
	QString saveIrisWidget(IrisWidget *iw, quint32 irisIndex=0, QString base="iris");
	void updateIdentityWidgets();
	
};

#endif // DEVELOPERWIDGET_HPP
