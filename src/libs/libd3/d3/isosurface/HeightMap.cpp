#include "HeightMap.hpp"

#include "uptime/New.hpp"

HeightMap::HeightMap(int w, int h)
	: w(w)
	, h(h)
	, data1(OC_NEW GLfloat[w*h])
	, data2(OC_NEW GLfloat[w*h])
{
	swap();
	fill(0.0f);
	swap();
	fill(0.0f);
	perturb(0.01);
}

QSize HeightMap::size(){
	return QSize(w, h);
}
