#ifndef IDENTICONLABEL_HPP
#define IDENTICONLABEL_HPP

#include <QLabel>
#include "identity/Identicon.hpp"

class IdenticonLabel : public QLabel
{
	Q_OBJECT
private:
	Identicon mIdenticon;

public:
	IdenticonLabel(QWidget *parent=nullptr);

public:
	void setPortableID(PortableID &id, bool showName=true, bool showIcon=true);
	Identicon identicon();

};

#endif // IDENTICONLABEL_HPP
