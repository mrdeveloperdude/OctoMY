#ifndef BLOBFUTURE_HPP
#define BLOBFUTURE_HPP

#include <QString>

class BlobFuture
{
private:
	const QString mName;
	QString mReason;
	bool mWin;
public:
	BlobFuture(QString name);


	void fail(QString reason);
	void win();

	const bool isWinning() const;
	const QString reason() const;
	const QString name() const;
};

#endif // BLOBFUTURE_HPP
