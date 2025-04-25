#ifndef PAIRINGTRUSTACTIVITY_H
#define PAIRINGTRUSTACTIVITY_H

#include "TrustLevel.hpp"
#include "components/navigation/Activity.hpp"
#include "uptime/ConfigureHelper.hpp"

#include <QTimer>
#include <QWidget>

class Node;
class QAbstractButton;
class Associate;

namespace Ui
{
class PairingTrustActivity;
}


class PairingTrustActivity : public Activity
{
	Q_OBJECT
private:
	Ui::PairingTrustActivity *ui;
	QSharedPointer<Node> mNode;
	ConfigureHelper mConfigureHelper;

public:
	explicit PairingTrustActivity(QWidget *parent = nullptr);
	virtual ~PairingTrustActivity() override;

public:
	void configure(QSharedPointer<Node> node);

private:
	TrustLevel trustLevel() const;
	void setTrustLevel(TrustLevel level);
	
	// Virtual activity mechanism
protected:
	void popImpl(const QString &returnActivity, const QStringList returnArguments = QStringList()) override;
	void pushImpl(const QStringList arguments = QStringList()) override;

private slots:
	void onTrustButtonClicked(QAbstractButton *);
	void finish(TrustLevel level);
	void saveTrust();
	void removeTrust();

};

#endif
// PairingTrustActivity_HPP
