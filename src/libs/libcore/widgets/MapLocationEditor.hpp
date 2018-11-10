#ifndef MAPLOCATIONEDITOR_HPP
#define MAPLOCATIONEDITOR_HPP

#include <QWidget>

namespace Ui
{
class MapLocationEditor;
}

class MapLocationEditor : public QWidget
{
	Q_OBJECT
private:
	Ui::MapLocationEditor *ui;
public:
	explicit MapLocationEditor(QWidget *parent = nullptr);
	~MapLocationEditor();

private slots:
	void on_pushButtonCancel_clicked();

	void on_pushButtonUse_clicked();

signals:

	void done(bool done);
};

#endif // MAPLOCATIONEDITOR_HPP
