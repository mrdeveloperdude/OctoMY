#include "MarkdownEditor.hpp"
#include "ui_MarkdownEditor.h"

#include "markdown/Markdown.hpp"
#include "markdown/MarkdownHighlighter.hpp"

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
#include <QStandardPaths>
#include <QTextDocument>
#include <QTextEdit>

namespace markdown{

MarkdownEditor::MarkdownEditor(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::MarkdownEditor)
{
	OC_METHODGATE();
	ui->setupUi(this);
	if(!connect(ui->plainTextEditMarkdown, SIGNAL(textChanged()), this, SLOT(onTextChanged()), OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect changed";
	}
	if(!connect(&saveTimer, SIGNAL(timeout()), this, SLOT(save()), OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect save";
	}
	/*
	TODO: Fix this
	auto uhl = OC_NEW UnionHighlighter(ui->plainTextEditMarkdown->document());
	uhl->registerHighlighter(OC_NEW MarkdownHighlighter(nullptr));
	uhl->registerHighlighter(OC_NEW HTMLHighlighter(nullptr));
	highlighter = uhl;
	*/
	highlighter = OC_NEW MarkdownHighlighter(ui->plainTextEditMarkdown->document());
	ui->plainTextEditMarkdown->setFocus();
	QString css=R"END(
table{
	background:red;
	border: 1px solid black;
}
)END";
	//ui->textEditPreview
	ui->textEditPreview->document()->setDefaultStyleSheet(css);
	saveTimer.setSingleShot(true);
	saveTimer.setTimerType(Qt::PreciseTimer);
	//ui->plainTextEditParseErrors->setVisible(false);
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
	plan_fn=p;
	//qDebug()<<"LOADING PLAN FROM FILE: "<<plan_fn;
	QSignalBlocker sb(ui->plainTextEditMarkdown);
	ui->plainTextEditMarkdown->setPlainText(utility::file::fileToString(plan_fn));
}


void MarkdownEditor::setText(QString txt)
{
	OC_METHODGATE();
	ui->plainTextEditMarkdown->setPlainText(txt);
}


void MarkdownEditor::toPDF(const QString &filename)
{
	OC_METHODGATE();
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


void MarkdownEditor::appendText(QString txt)
{
	OC_METHODGATE();
	ui->plainTextEditMarkdown->appendPlainText(txt);
}


void MarkdownEditor::onTextChanged()
{
	OC_METHODGATE();
	auto input = ui->plainTextEditMarkdown->toPlainText();
	
	Markdown md;
	auto output = md.process(input);
	
	qWarning()<<"IN:\n"<<input.size();
	qWarning()<<"OUT:\n"<<output.size();
	
	ui->textEditPreview->setHtml(output);
}


void MarkdownEditor::keyPressEvent(QKeyEvent *e)
{
	OC_METHODGATE();
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


void MarkdownEditor::on_pushButtonExportPDF_clicked()
{
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
