#ifndef LobeWIDGET_HPP
#define LobeWIDGET_HPP


#include <QWidget>


class LobeWidget : public QWidget
{
	Q_OBJECT
public:
	explicit LobeWidget(QWidget *parent = nullptr);

signals:

	void LobeDeleted(quint32);
};

#endif
// LobeWIDGET_HPP
