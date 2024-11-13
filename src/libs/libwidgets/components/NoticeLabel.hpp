#ifndef NOTICELABEL_HPP
#define NOTICELABEL_HPP

#include <QWidget>

namespace Ui {
class NoticeLabel;
}

class NoticeLabel : public QWidget
{
	Q_OBJECT
private:
	Ui::NoticeLabel *ui;
	bool mDeleteOnClose{false};
	
public:
	explicit NoticeLabel(QWidget *parent = nullptr);
	~NoticeLabel();
	
public slots:
	void notice(const QString &message);
	void setDeleteOnClose(bool deleteOnClose);
	void closeNotice();

};

#endif // NOTICELABEL_HPP
