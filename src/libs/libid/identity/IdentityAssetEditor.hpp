#ifndef IDENTITYASSETEDITOR_HPP
#define IDENTITYASSETEDITOR_HPP

#include <QWidget>
#include <QDir>

#include "uptime/ConfigureHelper.hpp"
#include "uptime/SharedPointerWrapper.hpp"

class Hub;
class IdenticonWidget;
class IrisWidget;
class LogDevice;
class Settings;

namespace Ui {
class IdentityAssetEditor;
}

class IdentityAssetEditor : public QWidget
{
	Q_OBJECT
private:
	Ui::IdentityAssetEditor *ui;
	const QDir mTopDir;
	QDir mLastDir;
	QSharedPointer<Hub> mHub;
	LogDevice *mLogDevice{nullptr};
	ConfigureHelper mConfigureHelper;
	
public:
	explicit IdentityAssetEditor(QWidget *parent = nullptr);
	~IdentityAssetEditor();
	
private:
	void appendLog(const QString &str);
private slots:
	void on_pushButtonRandomIdenticonID_clicked();
	
	void on_pushButtonSaveIdenticonAgent_clicked();
	void on_horizontalSliderIrisIndex_valueChanged(int value);
	void on_pushButtonSaveIdenticonIris_clicked();
	void on_pushButtonSaveIdenticonRemote_clicked();
	void on_pushButtonSaveIdenticonHub_clicked();
				   
public slots:
	void configure(QSharedPointer<Settings>  settings, QSharedPointer<Hub> hub=nullptr);
	QString saveIdenticonWidget(IdenticonWidget *iw, QString base="identicon");
	QString saveIrisWidget(IrisWidget *iw, quint32 irisIndex=0, QString base="iris");
	void updateIdentityWidgets();
	
	
};

#endif // IDENTITYASSETEDITOR_HPP
