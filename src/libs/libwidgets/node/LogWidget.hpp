#ifndef LOGWIDGET_HPP
#define LOGWIDGET_HPP

#include <QWidget>
#include <QPlainTextEdit>

#include "LogDevice.hpp"

namespace Ui
{
class LogWidget;
}

class LogWidget : public QPlainTextEdit, public LogDevice
{
	Q_OBJECT
private:
	Ui::LogWidget *ui;
	bool mScrollDirDown;

public:
	explicit LogWidget(QWidget *parent = nullptr);
	~LogWidget();

public:
	void setDirection(bool down) override;
	void appendLog(const QString& text) override;
};

#endif
// LOGWIDGET_HPP
