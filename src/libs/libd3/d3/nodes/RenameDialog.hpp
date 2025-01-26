#ifndef RENAMEDIALOG_H
#define RENAMEDIALOG_H

#include <QDialog>

namespace Ui {
class RenameDialog;
}

class RenameDialog : public QDialog
{
	Q_OBJECT
private:
	Ui::RenameDialog *ui;
	QString mName;

public:
	explicit RenameDialog(QWidget *parent = nullptr);
	~RenameDialog();
	
	void setName(QString name);
	QString name();

private slots:
	void finishRename();
	
};

#endif // RENAMEDIALOG_H
