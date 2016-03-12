#ifndef QT3DWINDOW_H
#define QT3DWINDOW_H

#include <QWindow>

class Qt3DWindow : public QWindow
{
	Q_OBJECT
public:
	explicit Qt3DWindow(QScreen *screen = 0);
	~Qt3DWindow();

};

#endif // QT3DWINDOW_H
