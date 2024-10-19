#ifndef IDENTICONLABEL_HPP
#define IDENTICONLABEL_HPP

#include <QWidget>

#include "identity/Identicon.hpp"

class QLabel;
class QHBoxLayout;


class IdenticonLabel : public QWidget
{
	Q_OBJECT
private:
	Identicon mIdenticon;
	QLabel *mIconLabel{nullptr};
	QLabel *mTextLabel{nullptr};
	QHBoxLayout *mLayout{nullptr};

public:
	IdenticonLabel(QWidget *parent=nullptr);
	
	
	// Ensure the widget is not smaller than its content
	QSize minimumSizeHint() const override;
	QSize sizeHint() const override;
	
public:
	void setPortableID(PortableID &id, bool showName = true, bool showIcon = true);
	Identicon identicon();

};

#endif // IDENTICONLABEL_HPP
