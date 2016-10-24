#ifndef CLIENTWINDOW_HPP
#define CLIENTWINDOW_HPP

#include <QWidget>
#include <QTimer>

#include "comms/Client.hpp"

#include "widgets/TryToggle.hpp"
#include "basic/Node.hpp"


#include <QSharedPointer>

class WaitingSpinnerWidget;

namespace Ui
{
class ClientWidget;
}

class ClientWidget : public QWidget
{
	Q_OBJECT

private:
	Ui::ClientWidget *ui;
	QTimer updateTimer;
	QSharedPointer<Node> mController;
	WaitingSpinnerWidget *mSpinner;

public:
	explicit ClientWidget(QWidget *parent = 0, QSharedPointer<Node> controller=QSharedPointer<Node>(nullptr));
	~ClientWidget();

private:
	bool eventFilter(QObject *object, QEvent *event);

	// Spinner
	void prepareSpinner();

	void init();

public:

	void updateControlLevel(int level);

	// Internal slots
public slots:
	void onSummaryTimer();
	void appendLog(const QString& text);


	// Internal UI slots
private slots:

	void onTryToggleConnectionChanged(TryToggleState);
	void on_pushButtonSay_clicked();

};

#endif // CLIENTWINDOW_HPP
