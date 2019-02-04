#ifndef ARDUINOPINCOMBOBOX_HPP
#define ARDUINOPINCOMBOBOX_HPP

#include <QComboBox>


class ArduinoPinCombobox : public QComboBox
{
public:
	ArduinoPinCombobox(QWidget *parent=nullptr);

signals:

	void arduinoPinChanged(quint8 pin);
};

#endif // ARDUINOPINCOMBOBOX_HPP
