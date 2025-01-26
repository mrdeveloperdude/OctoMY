#include "Player.hpp"

#include "Project.hpp"

Player::Player(QSharedPointer<Project> project)
	: QObject(nullptr)
	, mProject(project)
{
	connect(&mTimer, &QTimer::timeout, this, &Player::timeout);
	mTimer.setTimerType(Qt::PreciseTimer);
	mTimer.setInterval(1000/60);
}


bool Player::isRunning() const{
	return mIsRunning;
}


void Player::setRunning(bool running){
	mIsRunning = running;
	if (mIsRunning) {
		mElapsedTimer.start();
		mTimer.start();
	} else {
		mTimer.stop();
	}
}


void Player::timeout() {
	if (mIsRunning) {
		qreal dt = mElapsedTimer.restart() / 1000.0;
		mProject->step(dt);
		emit step(dt);
	}
}
