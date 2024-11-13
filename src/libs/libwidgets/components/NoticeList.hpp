#ifndef NOTICELIST_HPP
#define NOTICELIST_HPP

#include <QWidget>

class QVBoxLayout;

namespace Ui {
class NoticeList;
}

class NoticeList : public QWidget
{
	Q_OBJECT
private:
	Ui::NoticeList *ui;

public:
	explicit NoticeList(QWidget *parent = nullptr);
	~NoticeList();

public slots:
	void appendNotice(const QString &notice);
};

#endif // NOTICELIST_HPP
