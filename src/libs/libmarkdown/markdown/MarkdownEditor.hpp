#ifndef MARKDOWNEDITOR_HPP
#define MARKDOWNEDITOR_HPP

#include <QWidget>
#include <QTimer>
#include <QKeyEvent>
#include <QDir>
#include <QFileIconProvider>
#include <QFileSystemModel>


class OctomyParse;

class QSyntaxHighlighter;

namespace Ui
{
class MarkdownEditor;
}

namespace markdown{
Q_NAMESPACE

class MarkdownEditor : public QWidget
{
	Q_OBJECT
	
private:
	Ui::MarkdownEditor *ui;
	QSyntaxHighlighter *highlighter;
	QString plan_fn;
	QTimer saveTimer;
	QDir const topDir;
	QDir lastDir;
	QFileSystemModel filesystemModel;
	QFileIconProvider iconProvider;
	
public:
	explicit MarkdownEditor(QWidget *parent = nullptr);
	virtual ~MarkdownEditor() override;
	
public:
	void configure(QString plan_fn);
	void setText(QString);
	void toPDF(const QString &filename);
	void appendText(QString);
	
private:
	void hookUpFileSelector();
	void connectScrollbars();
	

protected:
	void keyPressEvent(QKeyEvent *e) override;
	
public slots:
	void onTextChanged();
	
private slots:
	void on_pushButtonExportPDF_clicked();
	
	void on_pushButtonToggleTree_toggled(bool checked);
	void on_pushButtonToggleSource_toggled(bool checked);
	void on_pushButtonTogglePreview_toggled(bool checked);
	void on_pushButtonToggleHTML_toggled(bool checked);
};

}

#endif
// MARKDOWNEDITOR_HPP

