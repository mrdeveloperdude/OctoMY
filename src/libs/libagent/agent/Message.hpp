#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <QWidget>
#include "components/navigation/Activity.hpp"

namespace Ui {
class Message;
}

class Message : public Activity
{
	Q_OBJECT

private:
	Ui::Message *ui;

public:
	explicit Message(QWidget *parent = nullptr);
	~Message();

public:
	void setMesasge(const QString &message);
};

#endif // MESSAGE_HPP
