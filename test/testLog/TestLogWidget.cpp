#include "TestLogWidget.hpp"

#include "log/LogWidget.hpp"
#include "log/LogStorage.hpp"

#include <QDate>
#include <QFile>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QListView>
#include <QPushButton>
#include <QVBoxLayout>

TestLogWidget::TestLogWidget(QWidget *parent)
	: QWidget(parent)
	, m_storage(QSharedPointer<LogStorage>::create())
{
	// 1) configure storage
	const QString logDir = "./test-logs";
	const qint64  maxSz  = 10 * 1024 * 1024;
	m_storage->configure(logDir, maxSz);
	
	// 2) build the LogWidget
	m_logWidget = new LogWidget;
	m_logWidget->configure(m_storage);
	
	// 3) create and hook up buttons
	m_lineEditAddLog    = new QLineEdit;
	m_btnAddLog         = new QPushButton("Add Log");
	m_btnToggleDirection = new QPushButton("Toggle Direction");
	m_btnToggleFilter    = new QPushButton("Toggle Filter");
	m_btnClearLogs       = new QPushButton("Clear Logs");
	m_btnReloadLogs      = new QPushButton("Reload Logs");
	
	connect(m_btnAddLog, &QPushButton::clicked, this, &TestLogWidget::addLog);
	connect(m_btnToggleDirection, &QPushButton::clicked, this, &TestLogWidget::toggleDirection);
	connect(m_btnToggleFilter, &QPushButton::clicked, this, &TestLogWidget::toggleFilter);
	connect(m_btnClearLogs, &QPushButton::clicked, this, &TestLogWidget::clearLogs);
	connect(m_btnReloadLogs, &QPushButton::clicked, this, &TestLogWidget::reloadLogs);
	
	// 4) layout: LogWidget on left, buttons stacked on right
	auto *btnLayout  = new QVBoxLayout;
	btnLayout->addWidget(m_lineEditAddLog);
	btnLayout->addWidget(m_btnAddLog);
	btnLayout->addSpacing(10);
	btnLayout->addWidget(m_btnToggleDirection);
	btnLayout->addWidget(m_btnToggleFilter);
	btnLayout->addWidget(m_btnClearLogs);
	btnLayout->addWidget(m_btnReloadLogs);
	btnLayout->addStretch();
	
	auto *mainLayout = new QHBoxLayout(this);
	mainLayout->addWidget(m_logWidget, 1);
	mainLayout->addLayout(btnLayout);
	
	setLayout(mainLayout);
}


void TestLogWidget::addLog()
{
	const QString text = m_lineEditAddLog->text();
	if (text.isEmpty()) {
		return;
	}
	m_storage->appendLog(text);
	m_lineEditAddLog->clear();
	m_lineEditAddLog->setFocus();
}


void TestLogWidget::toggleDirection()
{
	m_scrollDirDown = !m_scrollDirDown;
	m_logWidget->setLogDirection(m_scrollDirDown);
}

void TestLogWidget::toggleFilter()
{
	m_filterEnabled = !m_filterEnabled;
	m_logWidget->setEnableFilter(m_filterEnabled);
}

void TestLogWidget::clearLogs()
{
	if (!m_storage->clearLogs()) {
		qWarning() << "clearLogs() failed";
		return;
	}
}

void TestLogWidget::reloadLogs()
{
	if (!m_storage->reloadLogs()) {
		qWarning() << "reloadLogs() failed";
		return;
	}
}
