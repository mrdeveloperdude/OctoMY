#include "TestBlob.hpp"

#include "../libcore/comms/couriers/BlobCourier.hpp"


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
	const int numBlob=10;
	const quint32 chunkSize=480;
	QList<Blob *> blobs;
	for(int i=0; i<numBlob; ++i) {
		QByteArray aData=randomByteArray(qrand()%20000);
		Blob *blob=new Blob("blob_"+QString::number(i), aData, chunkSize, qrand()%30);
		blobs<<blob;
	}

	int go=0;
	while(!blobs.empty()) {
		qDebug()<<"";
		qDebug()<<"";
		qDebug()<<"------------------------------------------------------";
		int tot=0;
		int sent=0;
		int acked=0;
		for(Blob *blob:blobs) {
			const int back=blob->numAcknowleged();
			const int bsend=blob->numInTransit();
			const int btot=blob->numTotal();
			acked+=back;
			sent+=bsend;
			tot+=btot;
			qDebug()<<" + blob "<<blob->name()<<" total="<<btot<<",  sent="<<bsend<<",  acked="<<back<<", transit="<<blob->isInTransit()<<", ack="<<blob->isAcknowleged();
		}
		qDebug()<<"------------------------------------------------------";
		qDebug()<<"blobs "<<blobs.size()<<" total="<<tot<<",  sent="<<sent<<",  acked="<<acked<<", ";
		const quint32 index=qrand() % (blobs.count());
		Blob *blob=blobs[index];
		QVERIFY(nullptr!=blob);
		qDebug()<<"GO "<<(go++)<<" HAD INDEX "<<index;
		if(blob->isDone()) {
			qDebug()<<"BLOB '"<<blob->name()<<"' claims to be done, removing";
			blobs.removeAll(blob);
			delete blob;
			blob=nullptr;
		} else {
			QString name=blob->name();
			if(name.isEmpty()) {
				qWarning()<<"EMPTY NAME!";
			}
			BlobChunk chunk=blob->nextWorkItem();
			qDebug()<<"CHUNK "<<chunk.id()<<" IS NEXT";
			if(chunk.isValid()) {
				qDebug()<<"IT IS VALID";
				if(blob->isMyTurn()) {
					qDebug()<<"IT HAS ITS TURN";
					QByteArray data=chunk.data();
					qDebug()<<data.size() <<" BYTES OF DATA IS BEING "<<(chunk.isSent()?"RESENT":"SENT");
					chunk.setSent();
				}
			} else {
				qWarning()<<"ERROR: INVALID CHUNK FOUND";
			}

			//Acknowlege sent chunks with a probability of 10%
			const int ackChance=(qrand()%100);
			qDebug()<<"ackChance="<<ackChance;
			if(ackChance>20) {
				chunk=blob->chunk(qrand()%blob->numTotal());
				qDebug()<<"IN ACK WITH "<<chunk.id() << " SENT="<<chunk.isSent() << " ACKED="<<chunk.isAcknowleged();

				if(chunk.isSent() && !chunk.isAcknowleged() ) {
					qDebug()<<"CHUNK "<<chunk.id()<<" IS BEING ACKNOWLEGED";
					chunk.setAcknowleged();
				}
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
