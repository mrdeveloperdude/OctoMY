#ifndef FLATBUTTON_HPP
#define FLATBUTTON_HPP

#include <QPushButton>

class FlatButton: public QPushButton
{
	Q_OBJECT
public:
	explicit FlatButton(QWidget *parent = nullptr);
	~FlatButton();
	
	// QWidget interface:
protected:
	void paintEvent(QPaintEvent *e) override;
};

#endif // FLATBUTTON_HPP
