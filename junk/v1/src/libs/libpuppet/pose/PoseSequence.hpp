#ifndef POSESEQUENCE_HPP
#define POSESEQUENCE_HPP

#include "Pose.hpp"
#include <QMap>

class PoseSequence
{
private:
	//NOTE: QMap is inherently sorted by key which is good for us
	QMap<quint64, Pose> mPoses;
public:
	explicit PoseSequence();
	virtual ~PoseSequence();

public:

	void registerPose(quint64 time, Pose &pose)
	{
		mPoses[time]=pose;
	}

	quint64 before(quint64 time)
	{
		const quint64 sz=mPoses.size();
		if(sz<=0) {
			return 0;
		}
		QList<quint64> keys=mPoses.keys();
		quint64 last=keys[0];
		for(quint64 i=1; i<sz; ++i) {
			quint64 current=keys[i];
			if(time > last && time<current)	{
				return last;
			}
			last=current;
		}
	}


	quint64 after(quint64 time)
	{
		const quint64 sz=mPoses.size();
		if(sz<=0) {
			return 0;
		}
		QList<quint64> keys=mPoses.keys();
		quint64 last=keys[0];
		for(quint64 i=1; i<sz; ++i) {
			quint64 current=keys[i];
			if(time > last && time<current)	{
				return current;
			}
			last=current;
		}
	}

	Pose interpolate(quint64 time)
	{
		const quint64 sz=mPoses.size();
		if(sz<=0) {
			return Pose(0);
		}
		const quint64 b=before(time);
		const quint64 a=after(time);
		if(a==b) {
			return mPoses[a];
		}
		const qreal alpha=(qreal)(time-b)/(qreal)(a-b);
		Pose out=mPoses[b];
		out.mix(mPoses[a], alpha);
	}
};

#endif // POSESEQUENCE_HPP
