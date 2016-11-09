#ifndef SPEECHCONTROLWIDGET_HPP
#define SPEECHCONTROLWIDGET_HPP

#include <QWidget>

namespace Ui
{
class SpeechControlWidget;
}

class SpeechControlWidget : public QWidget
{
	Q_OBJECT

private:
	Ui::SpeechControlWidget *ui;

public:
	explicit SpeechControlWidget(QWidget *parent = 0);
	~SpeechControlWidget();

private slots:

	void appendSpeechHistory(const QString& text);

public slots:

	void on_pushButtonSay_clicked();
};

#endif // SPEECHCONTROLWIDGET_HPP
