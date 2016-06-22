#ifndef NETWORKSTATS_HPP
#define NETWORKSTATS_HPP

#include "StatsWindow.hpp"

class NetworkStats
{
	private:

		StatsWindow stats;
		QCPDataMap graphRTT;
		QCPDataMap graphSent;
		QCPDataMap graphReceived;
		QCPDataMap graphLost;
		QCPDataMap graphAcked;
		QCPDataMap graphSendBW;
		QCPDataMap graphReceiveBW;
		double randomWalk;

	public:
		explicit NetworkStats();
		virtual ~NetworkStats();

		void show();

		void appendGraphData(double rtt, int sent,int received,int lost,int acked, float sendBW,float receiveBW);


};

#endif // NETWORKSTATS_HPP
