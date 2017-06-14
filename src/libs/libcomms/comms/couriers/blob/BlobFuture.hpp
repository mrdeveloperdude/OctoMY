#ifndef BLOBFUTURE_HPP
#define BLOBFUTURE_HPP

#include <QString>


class BlobCourier;
class QObject;

class BlobFuture
{
private:
	BlobCourier *mCourier;
	const QString mName;
	QString mReason;
	bool mWin;
public:
	BlobFuture(QString name, BlobCourier *courier=nullptr);


	void fail(QString reason);
	void win();

	bool isWinning() const;
	const QString reason() const;
	const QString name() const;

	bool connect(QObject &recepient);
};

#endif // BLOBFUTURE_HPP
