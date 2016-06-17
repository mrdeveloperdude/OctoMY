#ifndef REMOTE_HPP
#define REMOTE_HPP

#include "sensory/SensorInput.hpp"
#include "comms/CommsChannel.hpp"
#include "comms/messages/SensorsMessage.hpp"

#include "puppet/Pose.hpp"

#include "basic/Node.hpp"
#include "basic/NodeLauncher.hpp"

#include <QObject>
#include <QVector2D>

class DirectPoseCourier;
class RemoteWindow;

class Remote : public Node{
		Q_OBJECT
	private:
		DirectPoseCourier *poseCourier;
		RemoteWindow *window;

	public:
		explicit Remote(NodeLauncher<Remote> &launcher, QObject *parent = nullptr);
		virtual ~Remote();

		void start(QHostAddress adrLocal, quint16 portLocal, QHostAddress hubAddress, quint16 hubPort);

		virtual QWidget *showWindow();

		//External commands
	public slots:


		void onDirectPoseChanged(Pose);

		//External sensors
	public slots:
		void onTouchUpdated(QVector2D);

};

#endif // REMOTE_HPP
