#include "TestBlob.hpp"

#include "../libcore/comms/couriers/BlobCourier.hpp"


/*

class Blob
{
private:

	QString mName;
	QByteArray mData;
	quint8 mPriority;

	int mTotalIndecies;
	int mLastAcknowlegedIndex;

	QMap<quint64, QSharedPointer<BlobChunk> > mInTransit;



public:
	explicit Blob(QString name, QByteArray data, quint8 priority);

public:

	bool isMyTurn();


	QSharedPointer<BlobChunk> nextWorkItem();


};



*/

static int ipow(int base, int exp)
{
	int result = 1;
	while (exp) {
		if (exp & 1) {
			result *= base;
		}
		exp >>= 1;
		base *= base;
	}

	return result;
}

static QByteArray randomByteArray(int size)
{
	QByteArray out;
	int bpc=(ipow(2,sizeof(char)*8));
	for(int i=0; i<size; ++i) {
		const char ch=(qrand() % bpc);
		out.append(ch);
	}
	return out;
}

// YOU NEED THIS: http://doc.qt.io/qt-5/qtest.html
void TestBlob::test()
{
	int numBlob=100;
	QList<Blob *> blobs;
	for(int i=0; i<numBlob; ++i) {
		QByteArray aData=randomByteArray(qrand()%1000);
		Blob *blob=new Blob("blob_"+QString::number(i), aData, qrand()%200);
		blobs<<blob;
	}

	while(!blobs.empty()) {
		const quint32 index=qrand() % numBlob;
		Blob *blob=blobs[index];
		if(blob->isDone()) {
			blobs.removeAll(blob);
			delete blob;
			blob=nullptr;
		} else if(blob->isMyTurn()) {
			QSharedPointer<BlobChunk> chunk=blob->nextWorkItem();
			QVERIFY(nullptr!=chunk);
			if(nullptr!=chunk) {
				QVERIFY((!chunk->isAcknowleged()));
				QByteArray data=chunk->data();
				qDebug()<<data.size() <<" BYTES OF DATA FOR CHUNK "<<chunk->id()<<" IS BEING "<<(chunk->isSent()?"RESENT":"SENT");
				chunk->setSent();
			}
		}
	}
	// Clean up (should not be necessary unless there is a premature exit as part of the test
	for(QList<Blob *>::iterator it=blobs.begin(), e=blobs.end(); it!=e; ++it) {
		delete *it;
	}
	blobs.clear();
}



QTEST_MAIN(TestBlob)
