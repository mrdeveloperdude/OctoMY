#ifndef CONSTRUCTSHELL_H
#define CONSTRUCTSHELL_H

#include <QDialog>

namespace Ui {
class ConstructShell;
}

class ConstructShell : public QDialog
{
	Q_OBJECT
private:
	Ui::ConstructShell *ui;
	
public:
	explicit ConstructShell(QWidget *parent = nullptr);
	~ConstructShell();
	
private:
	QWidget *hookHUD();
};

#endif // CONSTRUCTSHELL_H
