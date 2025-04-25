#ifndef WIDGETILLUSTRATIONEDITOR_HPP
#define WIDGETILLUSTRATIONEDITOR_HPP

#include "uptime/ConfigureHelper.hpp"
#include "uptime/SharedPointerWrapper.hpp"

#include <QDir>
#include <QWidget>

class Node;

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
	void configure(QSharedPointer<Node> node);
	
public slots:
	void on_pushButtonWidgetIllustrationBrowse_clicked();
	void on_pushButtonWidgetIllustrationSaveAll_clicked();
	
};

#endif // WIDGETILLUSTRATIONEDITOR_HPP
