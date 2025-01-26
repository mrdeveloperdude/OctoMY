#ifndef HEIGHTWIDGET_HPP
#define HEIGHTWIDGET_HPP


#include <qwidget.h>


class HeightMap;

class HeightWidget: public QWidget{
	Q_OBJECT
private:
	HeightMap *mMap;
	
public:
	HeightWidget(HeightMap *map);
	
	void updateSize();
	void paintEvent(QPaintEvent *) override;
};


#endif // HEIGHTWIDGET_HPP
