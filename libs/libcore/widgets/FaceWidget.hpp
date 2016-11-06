#ifndef FACEWIDGET_HPP
#define FACEWIDGET_HPP

#include "widgets/TryToggle.hpp"

#include <QWidget>



class Agent;

namespace Ui
{
class FaceWidget;
}

class FaceWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::FaceWidget *ui;

	QString lastID;

public:
	explicit FaceWidget(QWidget *parent = 0);
	~FaceWidget();

protected:

	Agent *agent();



public:
	void updateEyeColor();
	void updateVisibility();
	void appendLog(const QString&);
	void setAgent(Agent *);

	void setConnectionState(const TryToggleState s);


	void hookSignals(QObject &ob);
	void unHookSignals(QObject &ob);



signals:

	void connectionStateChanged(const TryToggleState last, const TryToggleState current);
	void colorChanged(QColor);

private slots:
	void on_pushButtonNewColor_clicked();
};

#endif // FACEWIDGET_HPP
