#include "NetworkStats.hpp"

#include <QDateTime>
NetworkStats::NetworkStats()
	: randomWalk(0.0)
{

}


void NetworkStats::show(){
	stats.addGraph("RTT",graphRTT,QPen(QBrush( QColor(0,192,128)), 2, Qt::SolidLine));
	stats.addGraph("Sent",graphSent, QPen(QBrush(QColor(192,128,0)), 2, Qt::SolidLine));
	stats.addGraph("Received",graphReceived, QPen(QBrush(QColor(128,0,192)), 2, Qt::DashLine));

	stats.addGraph("Acked",graphAcked,QPen(QBrush(QColor(0,192,0)), 2, Qt::DotLine));
	stats.addGraph("Lost",graphLost, QPen(QBrush(QColor(192,0,0)), 2, Qt::SolidLine));
	stats.addGraph("Send BW",graphSendBW,QPen(QBrush(QColor(192,0,128)), 2, Qt::SolidLine));
	stats.addGraph("Receive BW",graphReceiveBW,QPen(QBrush(QColor(0,128,192)), 2, Qt::SolidLine));
	stats.show();

}




/*
//RANDOM WALKS STUFF
graphRTT.clear();
	float randomWalk=0.0f;
	int n=100;
	for(int i=0;i<n;++i){
		graphRTT[i]=QCPData(i,randomWalk);
		randomWalk+=(qrand()/(float)RAND_MAX-0.5f)*0.1f;
	}
	stats.addGraph("Random data",graphRTT);

 */
void NetworkStats::appendGraphData(double rtt, int sent,int received,int lost,int acked, float sendBW,float receiveBW){
	double now=QDateTime::currentMSecsSinceEpoch()/1000.0;
	graphRTT[now]=QCPData(now,rtt);
	graphSent[now]=QCPData(now,sent);
	graphReceived[now]=QCPData(now,received);
	graphLost[now]=QCPData(now,lost);
	graphAcked[now]=QCPData(now,acked);
	graphSendBW[now]=QCPData(now,sendBW);
	graphReceiveBW[now]=QCPData(now,receiveBW);
	stats.repaintPlot();
}

