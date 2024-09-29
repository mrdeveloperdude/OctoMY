#ifndef QUITTING_HPP
#define QUITTING_HPP

#include <QWidget>

#include "components/navigation/Activity.hpp"

namespace Ui {
class Quitting;
}

class Quitting : public Activity
{
	Q_OBJECT
	
private:
	Ui::Quitting *ui;

public:
	explicit Quitting(QWidget *parent = nullptr);
	~Quitting();
private slots:
	
	void cancel();
	void doQuit();
	
signals:
	void done(bool qutting);

};

#endif // QUITTING_HPP
