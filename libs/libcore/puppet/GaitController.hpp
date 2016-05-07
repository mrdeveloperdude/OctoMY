#ifndef GAITCONTROLLER_HPP
#define GAITCONTROLLER_HPP

#include <QtGlobal>
#include <QDateTime>
#include <QDebug>
#include <QPainter>

template <typename T>
class GaitController;


template <typename T=quint16, T L=6>
class GaitSequenceGenerator{
	private:
		T lut[L];
		T i=0;
	public:

		enum GaitType{
			TRIPOD,
			WAVE,
			RIPPLE
		};

		explicit GaitSequenceGenerator(GaitType type=TRIPOD){
			switch(type){
				case(TRIPOD):{
						T a=0;
						T b=0;
						//Alternate odd/even limbs
						T L2=L/2;
						for(i=0;i<L;++i){
							a|=(1<<(i*2));
							b|=(2<<(i*2));
							lut[i]=0;
						}
						for(i=0;i<L;++i){
							lut[i]=(i&1)>0?a:b;
							//qDebug()<<"SEQ LUT ("<<i<<"): "<<  QString("%1").arg( lut[i], 16, 2, QChar('0'));
						}

					}break;
			}

		}

		//Returns integer flag where each bit represents one limb
		T generate(){
			i=(i+1)%L;
			T out= lut[i];
			//qDebug()<<"GATSEQ: "<< QString("%1").arg( out, 16, 2, QChar('0'));
			return out;
		}
};

const qreal LIMB_UNIT_LENGTH=0.2;

template <typename T>
class GaitLimb{
	public:
		GaitController<T> &body;
		quint32 id;
		const T LENGTH_COXA=LIMB_UNIT_LENGTH;
		const T LENGTH_FEMUR=LIMB_UNIT_LENGTH;
		const T LENGTH_TIBIA=LIMB_UNIT_LENGTH;
		T angle;
		T cz;//Foot distance over ground
		T bx,by;//Base position of limb relative to center of body
		T nx,ny;//Neutral position of foot relative to base
		T sx,sy;//Last foothold position in absolute ground plane coords
		T cx,cy;//Current foot position in absolute ground plane coords
		T tx,ty;//Target (Next) foothold in absolute ground plane coords
		T ox,oy;//Body position on cycle start (used to calculate relative body movement)
		T score;//Temporary variable used by gait planner to select best candidate limb for next cycle
		bool enabledPending;//The wanted value of enabled, held until an apropriate moment to switch
		bool enabled;//true if this limb is part of the gait. If not (for exanmple if it is busy with something else than walking or is damaged etc) the gait will adapt to manage without it.
		bool liftPending;//We are up for lifting next
		bool lift;//true if this limb is in a lift cycle
		bool balance; //true if this limb is part of balance calculation
		quint64 cycleStart; //timestamp of moment when this limb was put in lift cycle
		quint64 cycleTime; //length of one cycle in ms
		T minStride; //Shortest possible stride (in both distance and time
		T servoSpeed; //Speed of servo (assumes servos run continuously at max speed )
		T tibiaAngle;
		T coxaAngle;
		T femurAngle;




	public:
		GaitLimb(GaitController<T> &body, quint32 id, T bx,T by,T angle)
			: body(body)
			, id(id)
			, angle(angle)
			, cz(0)
			, bx(bx), by(by)
			, nx(bx+sin(angle)*0.2), ny(by+cos(angle)*0.2)
			, sx(nx), sy(ny)
			, cx(body.cx+nx), cy(body.cy+ny)
			, tx(nx), ty(ny)
			, ox(body.cx), oy(body.cy)
			, enabledPending(true)
			, enabled(enabledPending)
			, liftPending(false)
			, lift(liftPending)
			, balance(false)
			, cycleStart(0)
			, cycleTime(23370)
			, minStride(0.6)
			, servoSpeed(1000)
			, tibiaAngle(0)
			, coxaAngle(0)
			, femurAngle(0)
		{
		}

		void update(quint64 now, quint64 updateInterval){
			quint64 feedcycle=cycleTime/body.feedrate;
			if(lift){
				quint64 cycleInterval=now-cycleStart;
				if(cycleInterval>=feedcycle){
					const quint64 cyclesSinceLastUpdate=cycleInterval/(feedcycle+1);
					cycleInterval-=feedcycle*cyclesSinceLastUpdate;
					ox=body.cx;
					oy=body.cy;
					lift=false;
					cx=sx=tx;
					cy=sy=ty;
					//qDebug()<<"LIMB "<<id<<" touch down";
				}
				T cyclePosition=cycleInterval;
				cyclePosition/=((T)feedcycle);
				const T p=1.0-(cos(cyclePosition*M_PI)+1.0)*0.5;//Easing
				const T ip=1.0-p;
				cx=sx*ip+tx*p;
				cy=sy*ip+ty*p;
				cz=cos(M_PI*cyclePosition);
			}
			else if(!enabled){
				T t=updateInterval/1000.0;//sec
				T damp=pow(0.9,t);
				T idamp=1.0-damp;
				cx=cx*damp+body.cx*idamp;
				cy=cy*damp+body.cy*idamp;
			}

			const T ex=(body.cx+nx)-cx;
			const T ey=(body.cy+ny)-cy;
			score=sqrt(ex*ex+ey*ey);
			if(0==id){
				//qDebug()<<"SCORE:" <<score;
			}
			if(liftPending){
				liftPending=false;
				//Calculate target
				tx=nx+body.cx+((body.ddx*feedcycle*2)/updateInterval)+body.cobx/updateInterval;
				ty=ny+body.cy+((body.ddy*feedcycle*2)/updateInterval)+body.coby/updateInterval;
				//Calculate cycletime
				T stridex=cx-tx;
				T stridey=cy-ty;
				T stride=sqrt(stridex*stridex+stridey*stridey)+minStride;
				cycleTime=servoSpeed*stride;
				if(cycleTime>10){
					lift=true;
					cycleStart=now;
					//qDebug()<<"LIMB "<<id<<" lift with cyctime " <<cycleTime;
				}
			}
			enabled=enabledPending;
			IK();
		}

		//Inspired by https://github.com/TXBDan/BETH_Raspi/blob/master/ik.cpp
		void IK(){
			T dz=cz;
			T dx=cx-body.cx;
			T dy=cy-body.cy;
			T CoxaFootDist = sqrt( pow(dy, 2) + pow(dx, 2) );
			T IKSW = sqrt( pow(CoxaFootDist-LENGTH_COXA, 2) + pow(dz, 2) );
			T IKA1 = atan2( (CoxaFootDist - LENGTH_COXA) , dz );
			T IKA2 = acos( (pow(LENGTH_TIBIA, 2) - pow(LENGTH_FEMUR, 2) - pow(IKSW, 2) ) / (-2.0*IKSW*LENGTH_FEMUR) );
			tibiaAngle = acos( (pow(IKSW, 2) - pow(LENGTH_TIBIA, 2) - pow(LENGTH_FEMUR, 2)) / (-2.0*LENGTH_FEMUR*LENGTH_TIBIA) );
			coxaAngle=atan2( dy , dx) ;
			femurAngle= IKA1 + IKA2 ;
		}

		void paint(QPainter &painter){
//			const T w=painter.device()->width();
	//		const T h=painter.device()->height();
		//	const T s=0.5;

			//QPointF origo(w2,h2);
			QPointF origin(bx,by);
			QPointF neutral(nx,ny);
			QPointF foot(cx-body.cx,cy-body.cy);
			QPointF target(tx-body.cx,ty-body.cy);
			QPointF old(sx-body.cx,sy-body.cy);
			QPen base(QBrush(Qt::NoBrush),0.02);
			QPen white(base);
			white.setWidthF(0.04);
			white.setColor(QColor(enabled?(lift?"white":"gray"):"darkred"));
			QPen green(base);
			green.setWidthF(0.04);
			green.setColor(QColor("green"));
			QPen purple(base);
			purple.setWidthF(0.01);
			purple.setColor(QColor("purple").darker());
			QPen red(base);
			red.setWidthF(0.02);
			red.setColor(QColor(balance?"yellow":"red"));
			QPen teal(base);
			teal.setWidthF(0.01);
			teal.setColor(QColor("teal"));

			QPen blue(base);
			blue.setWidthF(0.01);
			blue.setColor(QColor("blue"));

			const T r=0.02;
			painter.setPen(teal);
			painter.drawEllipse(old,r,r);

			painter.setPen(blue);
			painter.drawEllipse(target,r,r);

			painter.setPen(purple);
			painter.drawEllipse(neutral,r*0.3,r*0.3);

			painter.setPen(red);
			painter.drawEllipse(foot,r,r);

			painter.setPen(white);
			painter.drawLine(origin,foot);

			painter.setPen(enabled?green:red);
			painter.drawEllipse(origin,r,r);

		}
};


class GaitControllerMessenger:
		public QObject
{
		Q_OBJECT
	public:
		explicit GaitControllerMessenger()
			:QObject(0)
		{

		}

		virtual ~GaitControllerMessenger(){

		}

	signals:

		void gaitUpdated();


};

template <typename T>
class GaitController{
	private:
		enum Mode{
			SINGLE
			, STAGGER

		};

	public:

		GaitSequenceGenerator<quint16, 6> seq;
		quint32 limbCount;
		quint32 maxLiftCount;
		quint64 tickTime; //tick==cycle makes no overlap, tick < cycle gives overlap
		quint64 lastTickStart;
		quint64 lastUpdate;
		quint32 currentLiftIndex;//The first limb of the lift set. Full set is calculated based on the values of symetric + limb.enabled + liftCount

		Mode mode;
		T stridelengthTarget; // --V
		T stridelength; //How fast the robot wants to move (stridelength). 1.0 means normal walk speed. 2.0 means run speed. 0.5 means "tripping"
		T feedrateTarget; // --V
		T feedrate; //How fast animation is progressing relative to "normal". <1.0 means slow motion. 0.0 means pause, 2.0 means double speed etc. NOTE: there is a limitation on how fast servos may respond and when the feedrate is high it might start acting strange (servo speed is not part of the calculation, it is simply assumed that servos are always fast enough

		T dxT,dyT; //Target direction vector
		T dx,dy; //Direction vector
		T ddx,ddy; //Actual speed
		T adx,ady; //Ajusted direction vector
		T cx,cy; //Current position
		T cogx,cogy; //Current center of gravity (kept at 0,0 for now)
		T cobx,coby; //Current center of balance (average position of 3 feet farthest from center of gravity)

		bool singleLimbMode;

		QVector<GaitLimb<T> *> limbs;


		GaitControllerMessenger msg;

	public:
		GaitController()
			: limbCount(6)
			, maxLiftCount(3)
			, tickTime(10000)
			, lastTickStart(0)
			, lastUpdate(0)
			, currentLiftIndex(0)
			, stridelengthTarget(0.0)
			, stridelength(stridelengthTarget)
			, feedrateTarget(1.0)
			, feedrate(feedrateTarget)
			, dxT(0), dyT(-1), dx(dxT), dy(dyT), adx(dx), ady(dy), ddx(dx), ddy(dy), cx(0), cy(0)
			, cogx(0), cogy(0)
			, cobx(0), coby(0)
			, singleLimbMode(false)
		{
			limbs.resize(limbCount+1);
			T a=(M_PI*2)/(limbCount*2);
			T s=0.2;
			for(int i=0;i<limbCount;++i){
				T bx=sin(a)*s*0.7;
				T by=cos(a)*s;
				limbs[i]=new GaitLimb<T>(*this,i,bx,by,a<M_PI?M_PI/2:(M_PI*3/2));
				a+=((M_PI*2.0)/(qreal)limbCount);
			}
		}


		void update(){

			// Update time-keeping
			const quint64 now=QDateTime::currentMSecsSinceEpoch();
			if(0==lastUpdate){
				lastUpdate=now-1;
			}
			const quint64 updateInterval=now-lastUpdate;
			lastUpdate=now;
			quint64 tickInterval=now-lastTickStart;

			// Calculate center of balance & center of gravity
			cobx=0;
			coby=0;
			cogx=0;
			cogy=0;
			quint32 cobct=0;
			quint32 cogct=0;
			for(quint32 i=0;i<limbCount;++i){
				if(!limbs[i]->enabled || !limbs[i]->enabledPending){
					continue;
				}
				cogx+=limbs[i]->cx;
				cogy+=limbs[i]->cy;
				cogct++;
				if(limbs[i]->lift){
					continue;
				}
				cobx+=limbs[i]->cx;
				coby+=limbs[i]->cy;
				cobct++;
			}
			cobx/=cobct;
			coby/=cobct;
			cobx-=cx;
			coby-=cy;

			cogx/=cogct;
			cogy/=cogct;
			cogx-=cx;
			cogy-=cy;

			T t=updateInterval/1000.0;//sec

			// Update feedrate
			T damp2=pow(0.95,t);
			T idamp2=1.0-damp2;
			feedrate=feedrateTarget*idamp2+feedrate*damp2;

			// Update stridelength
			T damp=pow(0.9,t);
			T idamp=1.0-damp;
			stridelength=stridelengthTarget*idamp+stridelength*damp;
			dx=dxT*idamp+dx*damp;
			dy=dyT*idamp+dy*damp;
			ddx=(feedrate*dx*(stridelength)*t);
			ddy=(feedrate*dy*(stridelength)*t);

			if(tickInterval>=tickTime){
				quint64 ct=tickInterval/tickTime;
				tickInterval-=tickTime*ct;
				lastTickStart=now;
			}

			// Count enabled and lifted limbs
			quint32 enabledLimbs=0;
			quint32 liftedLimbs=0;
			for(quint32 i=0;i<limbCount;++i){
				if(limbs[i]->enabled){
					enabledLimbs++;
				}
				if(limbs[i]->lift){
					liftedLimbs++;
				}
			}

			if(!singleLimbMode){
				const bool adaptive=false;
				// Adpative lift set selector
				if(adaptive){
					for(int s=0, e=maxLiftCount-liftedLimbs;s<e;++s){
						const int high=getLimbWithHighestError();
						if(high>=0){
							if(limbs[high]->score > stridelength*0.4){
								limbs[high]->liftPending=true;
							}
						}
					}
				}
				// Sequential lift set selector
				else{
					if(liftedLimbs<=0){
						quint16 liftSet=seq.generate();
						quint16 limb=1;
						for(quint32 i=0;i<limbCount;++i){
							if((liftSet & limb) >0){
								limbs[i]->liftPending=true;
							}
							limb<<=1;
						}
					}
				}
			}

			// Update limbs
			for(quint32 i=0;i<limbCount;++i){
				limbs[i]->update(now, updateInterval);
			}

			cx+=ddx;
			cy+=ddy;

			emit msg.gaitUpdated();
		}


		void hookSignals(QObject &o){
			if(!msg.connect(&msg,SIGNAL(gaitUpdated()), &o,SLOT(onGaitUpdated()))){
				qWarning()<<"ERRO: Could not connect";
			}
		}

		void unHookSignals(QObject &o){
			if(!msg.disconnect(&msg,SIGNAL(gaitUpdated()), &o,SLOT(onGaitUpdated()))){
				qWarning()<<"ERRO: Could not disconnect";
			}
		}

		int findBalancer(){
			T d=-1;
			int candidate=-1;
			int skipped=0;
			for(quint32 i=0;i<limbCount;++i){
				if(!limbs[i]->enabled || !limbs[i]->enabledPending){
					skipped++;
					continue;
				}
				if(limbs[i]->lift || limbs[i]->liftPending){
					skipped++;
					continue;
				}
				if(limbs[i]->balance){
					skipped++;
					continue;
				}

				T loned=-1;
				int candidate2=-1;
				for(quint32 ii=0;ii<limbCount-1;++ii){
					quint32 j=(ii+i+1)%limbCount;
					if(!limbs[i]->balance){
						//continue;
					}
					if(!limbs[j]->enabled || !limbs[j]->enabledPending){
						continue;
					}
					if(limbs[j]->lift || limbs[j]->liftPending){
						continue;
					}
					T lonex=(limbs[i]->cx-limbs[j]->cx);
					T loney=(limbs[i]->cy-limbs[j]->cy);
					T lone=sqrt(lonex*lonex+loney*loney);
					if(lone>0 && lone > loned){
						candidate2=i;
						loned=lone;
					}
				}
				if(-1!=candidate2){
					if(loned>d){
						d=loned;
						candidate=candidate2;
					}
				}
			}
			qDebug()<<"BALANCER IS: "<<candidate<<" WITH "<<d<<" ( skipped "<<skipped<<")";
			if(candidate>-1){
				limbs[candidate]->balance=true;
			}
			return candidate;
		}

		int getLimbWithHighestError(){
			T error=-1;
			int candidate=-1;
			int skipped=0;
			for(quint32 i=0;i<limbCount;++i){
				if(!limbs[i]->enabled || !limbs[i]->enabledPending){
					skipped++;
					continue;
				}
				if(limbs[i]->balance){
					skipped++;
					continue;
				}
				if(limbs[i]->lift || limbs[i]->liftPending){
					skipped++;
					continue;
				}
				T e=limbs[i]->score;
				if(e>0 && e > error){
					candidate=i;
					error=e;
				}
			}
			//qDebug()<<"HIGHEST ERROR IS: "<<candidate<<" WITH "<<error<<" ( skipped "<<skipped<<")";
			return candidate;
		}

		void getLimbVars(quint32 id, T &cox, T &fem, T &tib ){
			if(id<limbCount){

				cox=limbs[id]->coxaAngle;
				fem=limbs[id]->femurAngle;
				tib=limbs[id]->tibiaAngle;

				//	cox=fem=tib=limbs[id]->coxaAngle;
			}
		}


		void setDirection(T x, T y){
			T l=sqrt(x*x+y*y);
			if(l<=0){
				l=0.0000001;
			}
			dxT=x/l;
			dyT=y/l;
			if(l<10){
				l=0;
			}

			stridelengthTarget=l/300.0;
			if(stridelengthTarget>1.0){
				stridelengthTarget=1.0;
			}
			qDebug()<<"NEW STRIDE LENGTH: "<<stridelengthTarget;
		}

		void setTarget(T x, T y){
			if(singleLimbMode){
				T l=sqrt(x*x+y*y);
				if(l<=0){
					l=0.0000001;
				}
				dxT=x/l;
				dyT=y/l;
				l/=400;
				if(l>1){
					l=1;
				}
				dxT*=l;
				dyT*=l;

				limbs[0]->cx=dxT;
				limbs[0]->cy=dyT;
			}
		}

		void setFeedrate(T f){
			qDebug()<<"FEEDRATE: "<<f;
			feedrateTarget=f;
		}

		void toggleLimb(int limb){

			if(limb>=1 && limb<=limbCount){
				limb--;
				limbs[limb]->enabledPending=!limbs[limb]->enabledPending;
				qDebug()<<"TOGGLING LIMB "<<limb;
			}
		}
		void togglePause(){
			qDebug()<<"TOGGLING PAUSE";
			feedrateTarget=(feedrateTarget>0.1?0.0:1.0);
		}



		void toggleSingleLimb(){
			singleLimbMode=!singleLimbMode;
			qDebug()<<"TOGGLING SINGLE LIMB MODE TO "<<(singleLimbMode?"ON":"OFF");

		}



		void paint(QPainter &painter){
			painter.setRenderHint(QPainter::Antialiasing,true);
			const T w=painter.device()->width();
			const T h=painter.device()->height();
			QPen base(QBrush(Qt::NoBrush),2);

			QPen brown(base);
			brown.setWidthF(0.01);
			brown.setStyle(Qt::SolidLine);
			brown.setColor(QColor("brown"));

			QPen green(base);
			green.setWidthF(0.04);
			green.setColor(QColor("green"));

			QPen purple(base);
			purple.setWidthF(0.01);
			purple.setColor(QColor("purple").darker());

			painter.translate(w/2,h/2);
			qreal s=qMin(w,h);
			painter.scale(s,s);

			T step=0.02;
			T gx=cx/step;
			gx-=floor(gx);
			gx*=-step;
			T gy=cy/step;
			gy-=floor(gy);
			gy*=-step;
			painter.setPen(brown);
			for(T y=gy-h;y<h;y+=step){
				painter.drawLine(QPointF(0,y-1),QPointF(w,y+1));
			}
			for(T x=gx-w;x<w;x+=step){
				painter.drawLine(QPointF(x-1,0),QPointF(x+1,h));
			}
			if(singleLimbMode){
				limbs[0]->paint(painter);
			}
			else{

				for(quint32 i=0;i<limbCount;++i){
					limbs[i]->paint(painter);
				}

				const T r=0.01;
				QPointF cob(cobx,coby);
				painter.setPen(green);
				painter.drawEllipse(cob,r,r);

				QPointF cog(cogx,cogy);
				painter.setPen(purple);
				painter.drawEllipse(cog,r,r);
			}

		}

};



#endif // GAITCONTROLLER_HPP
