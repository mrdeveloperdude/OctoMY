#ifndef NODEWINDOW_HPP
#define NODEWINDOW_HPP

#include <QWidget>

class NodeWindow : public QWidget
{
	Q_OBJECT
public:
	explicit NodeWindow(QWidget *parent = nullptr);
		virtual ~NodeWindow();



private:
	void notifyAndroid(QString);
	void toastAndroid(QString);
};

#endif // NODEWINDOW_HPP
