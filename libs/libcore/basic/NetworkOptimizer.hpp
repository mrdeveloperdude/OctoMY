#ifndef NETWORKOPTIMIZER_HPP
#define NETWORKOPTIMIZER_HPP

#include <QNetworkAccessManager>

/**
  Provide a centralized location to perform optimizations as described in this link:

  http://devdays.kdab.com/wp-content/uploads/2013/11/speeding-up-your-Qt-app-with-new-QtNetwork-features.pdf

*/


class NetworkOptimizer
{
	private:
		static QNetworkAccessManager *nam;
	public:
		NetworkOptimizer();

	public:

		static QNetworkAccessManager &instance(QString host="http://zoo.octomy.org");
};

#endif // NETWORKOPTIMIZER_HPP
