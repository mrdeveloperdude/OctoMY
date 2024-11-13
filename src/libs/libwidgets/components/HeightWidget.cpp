#include "HeightWidget.hpp"


#include "HeightMap.hpp"

HeightWidget::HeightWidget(HeightMap *map)
	: QWidget(nullptr)
	, mMap(map)
{
	
}
	
void HeightWidget::updateSize(){
	auto last = rect();
	auto now = mMap->size();
	if(now!=last.size()){
		setGeometry(QRect(last.topLeft(), now));
	}
}


void HeightWidget::paintEvent(QPaintEvent *){
	QPainter painter(this);
	painter.fillRect(rect(), Qt::green);
	auto now = mMap->size();
	const auto w = now.width();
	const auto h = now.height();
	for(int y=0;y<h;++y){
		for(int x=0;x<w;++x){
			auto fx = static_cast<float>(x) / w;
			auto fy = static_cast<float>(y) / h;
			int v = static_cast<int>(mMap->sampleTiling(fx, fy)*120+127);
			painter.fillRect(x, y, 1, 1, QColor(v, v, v));
		}
	}
}
