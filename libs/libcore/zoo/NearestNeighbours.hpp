#ifndef NEARESTNEIGHBOURS_HPP
#define NEARESTNEIGHBOURS_HPP


#include <QMap>
#include <QDateTime>


struct NearestNode{
		QString id;
		qreal x;
		qreal y;
		quint64 ts;

		NearestNode(QString id, qreal x, qreal y )
			: x(x)
			, y(y)
			, id(id)
			, ts(QDateTime::currentMSecsSinceEpoch())
		{

		}

		qreal distanceTo(qreal x, qreal y){
			qreal dx=this->x-x;
			qreal dy=this->y-y;
			qreal d=(dx*dx+dy*dy);
			d=d>0?sqrt(d):0;
			return d;
		}


};


/**
  Helper class to keep a list of points and to return the subset of these points
  that are within a certain range of another point.

  This is used to find nodes in the vicinity of eachother
 */
class NearestNeighbours
{
	private:
		QMap<QString, NearestNode > all;

	public:
		NearestNeighbours();

		void add(QString id, qreal x, qreal y){
			all[id]=NearestNode(id,x,y);
		}


		QList <T*> findWithinRange(qreal x, qreal y, qreal r){
			QList <T * > out;
			for(auto it=all.begin(), e=all.end(); it!=e; ++it){
				NearestNode<T> t=(*it);
				if(t.distanceTo(x,y)<r){
					out.append(t);
				}
			}
			return out;
		}


		void prune(){
			quint64 now=QDateTime::currentMSecsSinceEpoch();
			for(auto it=all.begin(), e=all.end(); it!=e; ++it){
				NearestNode t=(*it);
				if(t.distanceTo(x,y)<r){
					out.append(t);
				}
			}

		}
};

#endif // NEARESTNEIGHBOURS_HPP
