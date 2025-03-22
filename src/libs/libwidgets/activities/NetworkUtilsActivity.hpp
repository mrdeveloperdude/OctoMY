#ifndef NETWORKUTILSACTIVITY_HPP
#define NETWORKUTILSACTIVITY_HPP

#include "components/TryToggleState.hpp"
#include "components/navigation/Activity.hpp"
#include "log/LogType.hpp"
#include "uptime/ConfigureHelper.hpp"
#include "uptime/SharedPointerWrapper.hpp"

#include <QHostInfo>

class Node;


namespace Ui {
class NetworkUtilsActivity;
}


class NetworkUtilsActivity : public Activity
{
	Q_OBJECT

private:
	Ui::NetworkUtilsActivity *ui;
	QSharedPointer<Node> mNode;
	ConfigureHelper mConfigureHelper;

public:
	explicit NetworkUtilsActivity(QWidget *parent = nullptr);
	~NetworkUtilsActivity();
	
private:
	void configureSend();
	void configureConnection();
public:
	void configure(QSharedPointer<Node> node);

private:
	void log(const QString &text, LogType type=LOG_TYPE_INFO) const;

	// Virtual activity mechanism
protected:
	void pushImpl(const QStringList arguments = QStringList()) override;
	void popImpl(const QString &returnActivity, const QStringList returnArguments = QStringList()) override;

private slots:
	void onRemoteHostLookupComplete(QHostInfo hi);
	void onListenStateChanged(const TryToggleState last, const TryToggleState current);
};

#endif // NETWORKUTILSACTIVITY_HPP
