#ifndef POSEVIEW_HPP
#define POSEVIEW_HPP

#include "pose/Pose.hpp"

#include <QWidget>


class PoseView: public QWidget
{
	Q_OBJECT

private:
	const Pose *mPose;

public:
	explicit PoseView(QWidget *parent=nullptr);
	virtual ~PoseView();

public slots:
	void poseChanged(const Pose &);

protected:
	void paintEvent(QPaintEvent *event) override;
};

#endif
// POSEVIEW_HPP
