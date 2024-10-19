#ifndef WIDGETILLUSTRATIONEDITOR_HPP
#define WIDGETILLUSTRATIONEDITOR_HPP

#include "uptime/ConfigureHelper.hpp"

#include <QWidget>
#include <QDir>

namespace Ui {
class WidgetIllustrationEditor;
}

class WidgetIllustrationEditor : public QWidget
{
	Q_OBJECT
private:
	Ui::WidgetIllustrationEditor *ui;
	const QDir mTopDir;
	QDir mWidgetIllustrationOutputDir;
	
	ConfigureHelper mConfigureHelper;
	

public:
	explicit WidgetIllustrationEditor(QWidget *parent = nullptr);
	~WidgetIllustrationEditor();
	
public:
	void configure();
	
public slots:
	void on_pushButtonWidgetIllustrationBrowse_clicked();
	void on_pushButtonWidgetIllustrationSaveAll_clicked();
	
};

#endif // WIDGETILLUSTRATIONEDITOR_HPP
