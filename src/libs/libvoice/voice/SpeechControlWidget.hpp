#ifndef SPEECHCONTROLWIDGET_HPP
#define SPEECHCONTROLWIDGET_HPP

#include <QWidget>

#include "uptime/SharedPointerWrapper.hpp"

class Node;

namespace Ui
{
class SpeechControlWidget;
}

class SpeechControlWidget : public QWidget
{
	Q_OBJECT

private:
	Ui::SpeechControlWidget *ui;
	QSharedPointer<Node> mNode;
public:
	explicit SpeechControlWidget(QWidget *parent = nullptr);
	~SpeechControlWidget();
	
public:
	void configure(QSharedPointer<Node> node);
private:
	void setSpeechAvailable(const bool available);

private slots:
	void appendSpeechHistory(const QString& text);

public slots:
	void on_pushButtonSay_clicked();
};

#endif
// SPEECHCONTROLWIDGET_HPP
