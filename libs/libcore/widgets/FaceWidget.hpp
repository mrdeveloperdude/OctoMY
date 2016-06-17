#ifndef FACEWIDGET_HPP
#define FACEWIDGET_HPP

#include <QWidget>


class Agent;

namespace Ui {
	class FaceWidget;
}

class FaceWidget : public QWidget
{
		Q_OBJECT
	private:
		Ui::FaceWidget *ui;

	public:
		explicit FaceWidget(QWidget *parent = 0);
		~FaceWidget();

	public:
		void updateVisibility();
		void appendLog(const QString&);
		void setAgent(Agent *);
};

#endif // FACEWIDGET_HPP
