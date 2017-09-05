#include "TestBlob.hpp"
#include "../testCommon/TestCommon.hpp"

#include "comms/couriers/blob/BlobCourier.hpp"
#include "comms/couriers/blob/Blob.hpp"
#include "comms/couriers/blob/BlobChunk.hpp"
#include "discovery/NodeAssociateStore.hpp"
#include "security/KeyStore.hpp"

#include "comms/couriers/CourierMandate.hpp"

#include "utility/widgets/PixViewer.hpp"
#include "utility/Utility.hpp"

#include "../testCommon/CourierTester.hpp"
#include "comms/CommsCarrierUDP.hpp"




void TestBlob::testBlob()
{
	QVERIFY(false);
	const int numBlob=5;
	const quint32 chunkSize=480;
	QList<SendingBlob *> sendingBlobs;
	for(int i=0; i<numBlob; ++i) {
		//QByteArray aData=randomByteArray(qrand()%200000);
		QByteArray aData=utility::randomJPEGByteArray(512,512,90);
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




class CourierTesterBlob: public CourierTester
{
//	Q_OBJECT;
private:
	PixViewer pixA;
	PixViewer pixB;
	int imageSize;
	QImage rimgA;
	QByteArray ridA;
	const QString blobName;

public:
	explicit CourierTesterBlob(CommsChannel &comms)
		: CourierTester(new BlobCourier(comms),new BlobCourier(comms), "FROM", "TO  ")
		, pixA(nullptr,"Original image")
		, pixB(nullptr,"Transport image")
		, imageSize(300)
		, blobName("first_blob")
	{

	}

public:
	void onTestInitImp() Q_DECL_OVERRIDE {
		pixA.show();
		pixB.show();
		rimgA=utility::randomImage(imageSize, imageSize);
		ridA=utility::imageToByteArray(rimgA);
		pixA.setImage(rimgA);
		CourierMandate beforeMandate=mFromCourier->mandate();
		qDebug()<<"before="<<beforeMandate.toString();
		BlobFuture ret=((BlobCourier *)mFromCourier)->submitSendingBlob(blobName,ridA,0.5);
		QVERIFY(ret.isWinning());
		QVERIFY(((BlobCourier *)mFromCourier)->totalSendingDataSize()>0);
		qDebug()<<"TOTAL SEND IS " << ((BlobCourier *)mFromCourier)->totalSendingDataSize();

		QObject::connect(((BlobCourier *)mToCourier), &BlobCourier::blobReceiveComplete, this, [this](QString name)
		{
			qDebug()<<"TEST DONE for "<<name;
			mDone=true;
		});

	}


	void onTestDeInitImp() Q_DECL_OVERRIDE {
		qDebug()<<"TEST DONE, waiting for final show-off...";
		QTest::qWait(3000);
		pixA.close();
		pixB.close();
		qApp->processEvents();
	}



	void onToReceivingImp() Q_DECL_OVERRIDE {
		QByteArray intermediateData=((BlobCourier *)mToCourier)->dataForReceivingBlob(blobName);
		if(intermediateData.size()>0)
		{
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

	}

	void onFromReceivingImp() Q_DECL_OVERRIDE {
	}

	void onToSendImp() Q_DECL_OVERRIDE {
	}

	void onFromSendImp() Q_DECL_OVERRIDE {
	}

};


void TestBlob::testBlobCourier2()
{
	KeyStore keystore;
	NodeAssociateStore peers;
	//CommsCarrier &carrier, KeyStore &keystore, NodeAssociateStore &peers, QObject *parent=nullptr);
	CommsCarrierUDP carrier;
	CommsChannel comms(carrier, keystore, peers);
	CourierTesterBlob blobTest(comms);
	blobTest.testRandomSteps(100);
}


QTEST_MAIN(TestBlob)
