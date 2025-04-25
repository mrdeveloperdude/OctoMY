// TestLogWidget.hpp
#ifndef TESTLOGWIDGET_HPP
#define TESTLOGWIDGET_HPP

#include <QWidget>
#include <QSharedPointer>

class LogWidget;
class LogStorage;
class QPushButton;
class QLineEdit;

class TestLogWidget : public QWidget {
	Q_OBJECT
	
private:  // data members first
	QSharedPointer<LogStorage>	m_storage;
	LogWidget				*m_logWidget{nullptr};
	bool					m_scrollDirDown{false};
	bool					m_filterEnabled{false};
	
	// right-hand buttons
	QLineEdit				*m_lineEditAddLog{nullptr};
	QPushButton				*m_btnAddLog{nullptr};
	QPushButton				*m_btnToggleDirection{nullptr};
	QPushButton				*m_btnToggleFilter{nullptr};
	QPushButton				*m_btnClearLogs{nullptr};
	QPushButton				*m_btnReloadLogs{nullptr};
	
public:
	explicit TestLogWidget(QWidget *parent = nullptr);
	
private slots:
	void addLog();
	void toggleDirection();
	void toggleFilter();
	void clearLogs();
	void reloadLogs();
};

#endif // TESTLOGWIDGET_HPP

