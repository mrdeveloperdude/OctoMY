#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <QTimer>
#include <QElapsedTimer>
#include <QSharedPointer>


class Project;

class Player: public QObject
{
	Q_OBJECT;
private:
	QSharedPointer<Project> mProject;
	bool mIsRunning{false};
	QTimer mTimer;
	QElapsedTimer mElapsedTimer;

public:
	Player(QSharedPointer<Project> project = nullptr);

public:
	bool isRunning() const;
	void setRunning(bool  running);

public slots:
	void timeout();
	
signals:
	void step(qreal dt);

};

#endif // PLAYER_HPP
