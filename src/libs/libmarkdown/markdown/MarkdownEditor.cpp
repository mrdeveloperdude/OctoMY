#include "MarkdownEditor.hpp"
#include "ui_MarkdownEditor.h"

#include "markdown/Markdown.hpp"
#include "markdown/MarkdownHighlighter.hpp"
//#include "markdown/HTMLHighlighter.hpp"

//#include "markdown/HTMLHighlighter.hpp"
//#include "markdown/UnionHighlighter.hpp"

#include "uptime/ConnectionType.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "utility/file/File.hpp"


#include <QDebug>
#include <QFileDialog>
#include <QPainter>
#include <QPrinter>
#include <QScroller>
#include <QStandardPaths>
#include <QTextDocument>
#include <QScrollBar>
#include <QTextEdit>

namespace markdown{


MarkdownEditor::MarkdownEditor(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::MarkdownEditor)
	, mConfigureHelper("MarkdownEditor", true, false, false, true, false)
{
	OC_METHODGATE();
	ui->setupUi(this);
	ui->textEditHTML->setReadOnly(true);
	ui->textEditPreview->setReadOnly(true);
}

MarkdownEditor::~MarkdownEditor()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
	delete highlighter;
	highlighter=nullptr;
}


void MarkdownEditor::configure(QString p)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()){
		
		
		connectScrollbars();
		if (!connect(ui->plainTextEditMarkdown, &QPlainTextEdit::textChanged, this, &MarkdownEditor::onTextChanged)) {
			qWarning() << "ERROR: Could not connect textChanged";
		}
		
		/*
	 * TODO: Why is there no save()? Whould it be onTextChanged()?
	if (!connect(&saveTimer, &QTimer::timeout, this, &MarkdownEditor::save)) {
		qWarning() << "ERROR: Could not connect save";
	}*/
		
		/*
	TODO: Fix this
	auto uhl = OC_NEW UnionHighlighter(ui->plainTextEditMarkdown->document());
	uhl->registerHighlighter(OC_NEW MarkdownHighlighter(nullptr));
	uhl->registerHighlighter(OC_NEW HTMLHighlighter(nullptr));
	highlighter = uhl;
	*/
		highlighter = OC_NEW MarkdownHighlighter(ui->plainTextEditMarkdown->document());
		//auto h2 = OC_NEW HTMLHighlighter(ui->textEditHTML->document());
		ui->plainTextEditMarkdown->setFocus();
		/* TODO: Load and apply CSS
	QString css=R"END(
table{
	background:red;
	border: 1px solid black;
}
)END";
	ui->textEditPreview->document()->setDefaultStyleSheet(css);
*/
		saveTimer.setSingleShot(true);
		saveTimer.setTimerType(Qt::PreciseTimer);
		//ui->plainTextEditParseErrors->setVisible(false);
		hookUpFileSelector();
		
		
		plan_fn=p;
		//qDebug()<<"LOADING PLAN FROM FILE: "<<plan_fn;
		QSignalBlocker sb(ui->plainTextEditMarkdown);
		ui->plainTextEditMarkdown->setPlainText(utility::file::fileToString(plan_fn));
	}
}


void MarkdownEditor::connectScrollbars(){
	if(mConfigureHelper.isConfiguredAsExpected()) {
		auto sb=ui->plainTextEditMarkdown->verticalScrollBar();
		if(nullptr!=sb){
			connect(sb, &QScrollBar::valueChanged, this, [=](int v){
				const auto m=sb->maximum();
				ui->textEditPreview->verticalScrollBar()->setValue( (ui->textEditPreview->verticalScrollBar()->maximum() * v) / m );
				ui->textEditHTML->verticalScrollBar()->setValue( (ui->textEditHTML->verticalScrollBar()->maximum() * v) / m );
				
			}, Qt::AutoConnection);
		}
	}
}


void MarkdownEditor::setText(QString txt)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		ui->plainTextEditMarkdown->setPlainText(txt);
	}
}


void MarkdownEditor::toPDF(const QString &filename)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		/*
		QPrinter printer(QPrinter::HighResolution);
		printer.setOutputFileName("print.pdf");
		ui->textEditPreview->print(&printer);
		*/
		QPrinter printer(QPrinter::HighResolution);
		printer.setOutputFileName(filename);
		QPainter painter;
		painter.begin(&printer);
		QTextEdit text;
		text.setText("PDF Export");
		text.print(&printer);
		printer.newPage();
		ui->textEditPreview->print(&printer);
	}
}


void MarkdownEditor::appendText(QString txt)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		ui->plainTextEditMarkdown->appendPlainText(txt);
	}
}


void MarkdownEditor::hookUpFileSelector(){
	if(mConfigureHelper.isConfiguredAsExpected()) {
		QString rootPath("/");
		filesystemModel.setIconProvider(&iconProvider);
		filesystemModel.setRootPath(rootPath);
		//model.setOption(QFileSystemModel::DontUseCustomDirectoryIcons);
		filesystemModel.setOption(QFileSystemModel::DontWatchForChanges);
		auto tree=ui->treeViewFilesystem;
		tree->setModel(&filesystemModel);
		if (!rootPath.isEmpty()) {
			const QModelIndex rootIndex = filesystemModel.index(QDir::cleanPath(rootPath));
			if (rootIndex.isValid()){
				tree->setRootIndex(rootIndex);
			}
		}
		// Demonstrating look and feel features
		tree->setAnimated(false);
		tree->setIndentation(20);
		tree->setSortingEnabled(true);
		const QSize availableSize = tree->screen()->availableGeometry().size();
		tree->resize(availableSize / 2);
		tree->setColumnWidth(0, tree->width() / 3);
		
		// Make it flickable on touchscreens
		QScroller::grabGesture(tree, QScroller::TouchGesture);
	}
}


void MarkdownEditor::onTextChanged()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		// HACK: This method crashes because destructor is called prematurely (or onTextChange() too late?) and so ui is 0
		if(nullptr==ui){
			return;
		}
		auto input = ui->plainTextEditMarkdown->toPlainText();
		
		Markdown md;
		auto output = md.process(input);
		
		qWarning()<<"IN:\n"<<input.size();
		qWarning()<<"OUT:\n"<<output.size();
	
		ui->textEditHTML->setPlainText("HTML <b>BOLD</b>\n"+output);
		ui->textEditPreview->setHtml("HTML <b>BOLD</b>\n"+output);
		//QTextDocument td;td.setHtml(output);
		//ui->textEditPreview->setDocument(&td);
	}
}


void MarkdownEditor::keyPressEvent(QKeyEvent *e)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		Qt::Key key=static_cast<Qt::Key>(e->key());
		//qDebug()<<"KEY:"<<key;
		//Handle some stdandard keyboard shortcuts for text editor
		if(e->matches(QKeySequence::Save)) {
			e->accept();
			//save();
			return;
		} else if(e->matches(QKeySequence::New)) {
			e->accept();
			ui->plainTextEditMarkdown->setPlainText("");
			return;
		} else if((Qt::Key_F==key) && (e->modifiers()==( Qt::ShiftModifier | Qt::ControlModifier))) {
			e->accept();
			//tidy();
			return;
		}
	}
}


void MarkdownEditor::exportPDF()
{
	if(mConfigureHelper.isConfiguredAsExpected()) {
		QFileDialog fd;
		QStringList filters;
		QString filePath="";
		if(topDir == lastDir) {
			lastDir.setPath(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));
		}
		fd.setDirectory(lastDir);
		//filters << "PNG (*.png)";
		//filters << "SVG (*.svg)";
		filters << "PDF (*.pdf)";
		fd.setNameFilters(filters);
		fd.selectFile(filePath);
		fd.setFileMode(QFileDialog::AnyFile);
		fd.setAcceptMode(QFileDialog::AcceptSave);
		if (fd.exec()) {
			lastDir=fd.directory();
			QStringList  fileNames = fd.selectedFiles();
			if(fileNames.size()>0) {
				QString fileName=fileNames.first();
				QFileInfo fileInfo(fileName);
				if(fileInfo.fileName().endsWith(".pdf") && fileInfo.isWritable()) {
					toPDF(fileInfo.absoluteFilePath());
				}else{
					qWarning()<<"Only PDF supported at this time";
				}
				
				/*
				if(fileInfo.fileName().endsWith(".svg")) {
					QByteArray xml=iw->svgXML();
					filePath=fileInfo.absoluteFilePath();
					QFile file(filePath);
					file.open(QIODevice::WriteOnly);
					file.write(xml);
					file.close();
				} else if(fileInfo.fileName().endsWith(".png")) {
					Identicon identicon=iw->identicon();
					const quint32 sz=1024;
					QImage image=identicon.image(sz, sz);
					filePath=fileInfo.absoluteFilePath();
					image.save(filePath);
				}
				*/
			}
		}
	}
}


void MarkdownEditor::toggleTree(bool checked)
{
	if(mConfigureHelper.isConfiguredAsExpected()) {
		ui->treeViewFilesystem->setVisible(checked);
	}
}


void MarkdownEditor::toggleSource(bool checked)
{
	if(mConfigureHelper.isConfiguredAsExpected()) {
		ui->plainTextEditMarkdown->setVisible(checked);
	}
}


void MarkdownEditor::togglePreview(bool checked)
{
	if(mConfigureHelper.isConfiguredAsExpected()) {
		ui->textEditPreview->setVisible(checked);
	}
}


void MarkdownEditor::toggleHTML(bool checked)
{
	if(mConfigureHelper.isConfiguredAsExpected()) {
		ui->textEditHTML->setVisible(checked);
	}
}

}
