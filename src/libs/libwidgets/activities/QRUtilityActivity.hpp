#ifndef QRUTILITYACTIVITY_HPP
#define QRUTILITYACTIVITY_HPP

#include <QWidget>
#include "components/navigation/Activity.hpp"

class Node;

namespace Ui {
class QRUtilityActivity;
}


class QRUtilityActivity : public Activity
{
	Q_OBJECT

private:
	Ui::QRUtilityActivity *ui;
	
	
public:
	explicit QRUtilityActivity(QWidget *parent = nullptr);
	~QRUtilityActivity();
	
public:
	void configure(QSharedPointer<Node> node=nullptr);

	// Virtual activity mechanism
protected:
	void pushImpl(const QStringList arguments = QStringList()) override;
	void popImpl(const QString &returnActivity, const QStringList returnArguments = QStringList()) override;
	


};

#endif // QRUTILITYACTIVITY_HPP
