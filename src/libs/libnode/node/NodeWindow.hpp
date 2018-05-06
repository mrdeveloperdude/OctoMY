#ifndef NODEWINDOW_HPP
#define NODEWINDOW_HPP

#include <QWidget>


#include <QSharedPointer>
class Node;

class NodeWindow : public QWidget
{
	Q_OBJECT
private:

	QSharedPointer<Node> mNode;
public:
	explicit NodeWindow(QSharedPointer<Node> node, QWidget *parent = nullptr);
	virtual ~NodeWindow();


	// Desktop stuff
protected:

	void loadWindowGeometry();
	void saveWindowGeometry();


	// Android stuff
protected:
	void notifyAndroid(QString);
	void toastAndroid(QString);
};

#endif // NODEWINDOW_HPP
