#include "TestBlob.hpp"

#include "../libcore/comms/couriers/BlobCourier.hpp"
#include "../libcore/comms/couriers/Blob.hpp"
#include "../libcore/comms/couriers/BlobChunk.hpp"

#include "../libcore/comms/couriers/CourierMandate.hpp"

#include "../libutil/utility/widgets/PixViewer.hpp"
#include "../libutil/utility/Utility.hpp"

#include <QImage>
#include <QPainter>
#include <QImageWriter>
#include <QBuffer>


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


static QImage randomImage(int w=512, int h=512 )
{
	QImage image( w,h, QImage::Format_ARGB32);
	image.fill(Qt::green);
	{
		QPainter painter(&image);
		painter.setPen(Qt::NoPen);
		const int rr=(qMin(w,h)/16);
		const int area=(w*h)/rr;

		for(int i=0; i<area; ++i) {
			const QColor c(qrand()%255, qrand()%255, qrand()%255,127);
			const int r=qrand()%rr;
			const int x=qrand()%w;
			const int y=qrand()%h;
			painter.setBrush(c);
			painter.drawEllipse(x-r,y-r,r*2,r*2);
		}
	}
	return image;
}

static QByteArray imageToByteArray(QImage &image )
{
	const uchar *bits=image.bits();
	QByteArray ba((const char *)(bits), image.byteCount());
	return ba;
}

static QByteArray randomImageByteArray(int w=512, int h=512 )
{
	QImage image=randomImage(w,h);
	QByteArray ba=imageToByteArray(image);
	return ba;
}

static QByteArray randomJPEGByteArray(int w=512, int h=512, int q=85 )
{
	QImage image=randomImage(w,h);
	QBuffer bu;
	bu.open(QBuffer::WriteOnly);
	QImageWriter iw;
	iw.setFormat("jpeg");
	iw.setDevice(&bu);
	iw.setOptimizedWrite(true );
	iw.setQuality(q);
	iw.setCompression(9);
	iw.setProgressiveScanWrite(true );
	iw.write( image );
	bu.close();
	QByteArray ba=bu.buffer();
	//utility::byteArrayToFile("random_test.jpeg",ba );
	//qDebug()<<"Random JPEG image of size "<<w<<"x"<<h<<" was "<< utility::humanReadableSize(ba.size())<< "("<<(qreal)ba.size()/((qreal)w*h) << " bytes/pixel)";
	return ba;

}




void TestBlob::testBlob()
{
	QVERIFY(false);
	const int numBlob=5;
	const quint32 chunkSize=480;
	QList<SendingBlob *> sendingBlobs;
	for(int i=0; i<numBlob; ++i) {
		//QByteArray aData=randomByteArray(qrand()%200000);
		QByteArray aData=randomJPEGByteArray(512,512,90);
		SendingBlob *blob=new SendingBlob("blob_"+QString::number(i), i+1, chunkSize, aData, qrand()%30);
		sendingBlobs<<blob;
	}


	int go=0;
	while(!sendingBlobs.empty()) {
		qDebug()<<"";
		qDebug()<<"";
		qDebug()<<"------------------------------------------------------";
		int tot=0;
		int sent=0;
		int acked=0;
		for(SendingBlob *blob:sendingBlobs) {
			const int back=blob->numAcknowleged();
			const int bsend=blob->numSent();
			const int btot=blob->numTotal();
			acked+=back;
			sent+=bsend;
			tot+=btot;
			qDebug()<<" + sending blob "<<(blob->isDone()?"DONE":"----")<<" "<<blob->name()<<(blob->isNameAcknowleged()?"[ACK]":(blob->isNameSent()?"[SEND]":"[NONE]") )<<" total="<<btot<<",  sent="<<bsend<<",  acked="<<back<<", transit="<<blob->isSent()<<", ack="<<blob->isAcknowleged();
		}
		qDebug()<<"------------------------------------------------------";
		qDebug()<<"blobs "<<sendingBlobs.size()<<" total="<<tot<<",  sent="<<sent<<",  acked="<<acked<<", ";
		//QVERIFY(false);
		const quint32 index=qrand() % (sendingBlobs.count());
		SendingBlob *blob=sendingBlobs[index];
		QVERIFY(nullptr!=blob);
		qDebug()<<"GO "<<(go++)<<" HAD INDEX "<<index;
		if(blob->isDone()) {
			qDebug()<<"BLOB '"<<blob->name()<<"' claims to be done, removing";
			sendingBlobs.removeAll(blob);
			delete blob;
			blob=nullptr;
		} else {
			QString name=blob->name();
			if(name.isEmpty()) {
				qWarning()<<"EMPTY NAME!";
			}
			SendingBlobChunk chunk=blob->findNextSendingChunk();
			qDebug()<<"CHUNK "<<chunk.id()<<" IS NEXT";
			if(chunk.isValid()) {
				qDebug()<<"IT IS VALID";
				if((qrand()%100) > 50) {
					qDebug()<<"IT HAS ITS TURN";
					QByteArray data=chunk.data();
					qDebug()<<data.size() <<" BYTES OF DATA IS BEING "<<(chunk.isSent()?"RESENT":"SENT");
					chunk.setSent();
				}
			} else {
				qWarning()<<"ERROR: INVALID CHUNK FOUND";
			}

			//Progress name send with a set % probability
			const int nameChance=(qrand()%100);
			qDebug()<<"nameChance="<<nameChance;
			if(nameChance>80) {
				if(!blob->isNameSent()) {
					blob->setNameSent(true);
				} else if(!blob->isNameAcknowleged()) {
					blob->setNameAcknowleged(true);
				}
			}
			//Acknowlege chunk send with a set % probability
			const int ackChance=(qrand()%100);
			qDebug()<<"ackChance="<<ackChance;
			if(ackChance>60) {
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
	for(QList<SendingBlob *>::iterator it=sendingBlobs.begin(), e=sendingBlobs.end(); it!=e; ++it) {
		delete *it;
	}
	sendingBlobs.clear();
}



void TestBlob::testBlobCourier()
{
	PixViewer pixA(nullptr,"Original image");
	pixA.show();
	PixViewer pixB(nullptr,"Transport image");
	pixB.show();
	const int imageSize=300;
	QImage rimgA=randomImage(imageSize, imageSize);
	QByteArray ridA=imageToByteArray(rimgA);
	pixA.setImage(rimgA);
	BlobCourier *fromCourier=new BlobCourier();
	QVERIFY(nullptr!=fromCourier);
	BlobCourier *toCourier=new BlobCourier();
	QVERIFY(nullptr!=toCourier);
	CourierMandate beforeMandate=fromCourier->mandate();
	qDebug()<<"before="<<beforeMandate;
	const QString blobName="first_blob";
	BlobFuture ret=fromCourier->submitSendingBlob(blobName,ridA,0.5);
	QVERIFY(ret.isWinning());
	QVERIFY(fromCourier->totalSendingDataSize()>0);
	qDebug()<<"TOTAL SEND IS " << fromCourier->totalSendingDataSize();
	// Efforts made in sendingOpportunity and dataReceived are stored here and retreived at random to simulate the delay of network
	QList<QByteArray *> fromStreams;
	QList<QByteArray *> toStreams;
	bool done=false, *donep=&done;;
	connect(toCourier, &BlobCourier::blobReceiveComplete, this, [donep](QString name) {
		qDebug()<<"TEST DONE for "<<name;
		*donep=true;
	});
	while(!done) {
		//qDebug()<<"";		qDebug()<<"######################################## LOOP FROM="<<fromStreams.size()<<" TO="<<toStreams.size();
		// Give each party completely random chance of TX/RX to simulate some pretty bad network conditions
		const int chance=qrand()%5;
		switch(chance) {
		case(0): {
			QByteArray *ba=new QByteArray;
			QVERIFY(nullptr!=ba);
			quint16 sent=0;
			{
				QBuffer buf(ba);
				buf.open(QBuffer::WriteOnly);
				QDataStream stream(&buf);
				sent=fromCourier->sendingOpportunity(stream);
				buf.close();
			}
			//qDebug()<<" from sendop "<<sent<<" bytes ("<<ba->size()<<")";
			fromStreams<<ba;
		}
		break;
		case(1): {
			QByteArray *ba=new QByteArray;
			QVERIFY(nullptr!=ba);
			quint16 sent=0;
			{
				QBuffer buf(ba);
				buf.open(QBuffer::WriteOnly);
				QDataStream stream(&buf);
				sent=toCourier->sendingOpportunity(stream);
				buf.close();
			}
			//qDebug()<<" to sendop "<<sent<<" bytes ("<<ba->size()<<")";
			toStreams<<ba;
		}
		break;
		case(2): {
			if(!fromStreams.isEmpty()) {
				const int count=fromStreams.size();
				const int index=qrand()%count;
				QByteArray *ba=fromStreams.takeAt(index);
				QVERIFY(nullptr!=ba);
				const qint64 sz=ba->size();
				quint16 received=0;
				{
					QBuffer buf(ba);
					buf.open(QBuffer::ReadOnly);
					QDataStream stream(&buf);
					received=toCourier->dataReceived(stream, sz);
					buf.close();
				}
				QByteArray intermediateData=toCourier->dataForReceivingBlob(blobName);
				if(intermediateData.size()>0) {
					//qDebug()<<" X X X X X UPDATING IMAGE";
					QImage img(imageSize, imageSize, QImage::Format_ARGB32);
					char *bits=(char *)img.bits();
					int bytes=img.byteCount();
					for(int i=0; i<bytes; ++i) {
						bits[i]=intermediateData[i];
					}
					pixB.setImage(img);
					qApp->processEvents();
				}
				delete ba;
				ba=nullptr;
				//qDebug()<<" to rec "<<received<<" bytes vs "<<sz;
			} else {
				//qDebug()<<"NO-OP 1";
			}
		}
		break;
		case(3): {
			if(!toStreams.isEmpty()) {
				const int count=toStreams.size();
				QVERIFY(count>0);
				const int index=qrand()%count;
				QByteArray *ba=toStreams.takeAt(index);
				QVERIFY(nullptr!=ba);
				const qint64 sz=ba->size();
				quint16 received=0;
				{
					QBuffer buf(ba);
					buf.open(QBuffer::ReadOnly);
					QDataStream stream(&buf);
					received=fromCourier->dataReceived(stream, sz);
					buf.close();
				}
				delete ba;
				ba=nullptr;
				//qDebug()<<" from rec "<<received<<" bytes vs "<<sz;
			} else {
				// qDebug()<<"NO-OP 2";
			}
		}
		break;
		default:
		case(4):	{
			const int chance2=(qrand()%6);
			if(0==chance2) {
				// qDebug()<<"from remove";
			} else if(0==chance2) {
				// qDebug()<<"to remove";
			} else {
				// qDebug()<<"NO-OP 3";
			}
		}
		break;
		}
		//QVERIFY(false);

		//fromCourier->printSendingSummary("FROM");
	//	toCourier->printSendingSummary("TO");

	}
	qDebug()<<"TEST DONE, waiting for final show-off...";
	QTest::qWait(3000);
	pixA.close();
	pixB.close();
	qApp->processEvents();
}

QTEST_MAIN(TestBlob)
