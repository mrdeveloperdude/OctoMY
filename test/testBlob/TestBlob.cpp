#include "TestBlob.hpp"

#include "test/CourierTester.hpp"

#include "comms/couriers/blob/BlobCourier.hpp"
#include "comms/couriers/blob/Blob.hpp"
#include "comms/couriers/blob/BlobChunk.hpp"
#include "discovery/AddressBook.hpp"
#include "security/KeyStore.hpp"

#include "comms/couriers/CourierMandate.hpp"

#include "utility/graphics/PixViewerWidget.hpp"
#include "utility/graphics/Graphics.hpp"
#include "utility/random/Random.hpp"

#include "comms/CommsCarrierUDP.hpp"


#include <QByteArray>
#include <QSharedPointer>

void TestBlob::testBlob()
{
	//QVERIFY(false);
	const int numBlob=5;
	const quint32 chunkSize=480;
	QList<SendingBlob *> sendingBlobs;
	for(int i=0; i<numBlob; ++i) {
		//QByteArray aData=randomByteArray(utility::random::qrand()%200000);
		QByteArray aData=utility::graphics::randomJPEGByteArray(512,512,90);
		SendingBlob *blob=OC_NEW SendingBlob("blob_"+QString::number(i), i+1, chunkSize, aData, utility::random::qrand()%30);
		sendingBlobs<<blob;
	}


	int go=0;
	while(!sendingBlobs.empty()) {
		//qDebug()<<"";
		//qDebug()<<"";
		//qDebug()<<"------------------------------------------------------";
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
			//qDebug()<<" + sending blob "<<(blob->isDone()?"DONE":"----")<<" "<<blob->name()<<(blob->isNameAcknowleged()?"[ACK]":(blob->isNameSent()?"[SEND]":"[NONE]") )<<" total="<<btot<<",  sent="<<bsend<<",  acked="<<back<<", transit="<<blob->isSent()<<", ack="<<blob->isAcknowleged();
		}
		//qDebug()<<"------------------------------------------------------";
		//qDebug()<<"blobs "<<sendingBlobs.size()<<" total="<<tot<<",  sent="<<sent<<",  acked="<<acked<<", ";
		//QVERIFY(false);
		const quint32 index=utility::random::qrand() % (sendingBlobs.count());
		SendingBlob *blob=sendingBlobs[index];
		QVERIFY(nullptr!=blob);
		//qDebug()<<"GO "<<(go++)<<" HAD INDEX "<<index;
		if(blob->isDone()) {
			//qDebug()<<"BLOB '"<<blob->name()<<"' claims to be done, removing";
			sendingBlobs.removeAll(blob);
			delete blob;
			blob=nullptr;
		} else {
			QString name=blob->name();
			if(name.isEmpty()) {
				qWarning()<<"EMPTY NAME!";
			}
			SendingBlobChunk chunk=blob->findNextSendingChunk();
			//qDebug()<<"CHUNK "<<chunk.id()<<" IS NEXT";
			if(chunk.isValid()) {
				//qDebug()<<"IT IS VALID";
				if((utility::random::qrand()%100) > 50) {
					//qDebug()<<"IT HAS ITS TURN";
					QByteArray data=chunk.data();
					//qDebug()<<data.size() <<" BYTES OF DATA IS BEING "<<(chunk.isSent()?"RESENT":"SENT");
					chunk.setSent();
				}
			} else {
				//qWarning()<<"ERROR: INVALID CHUNK FOUND";
			}

			//Progress name send with a set % probability
			const int nameChance=(utility::random::qrand()%100);
			//qDebug()<<"nameChance="<<nameChance;
			if(nameChance>80) {
				if(!blob->isNameSent()) {
					blob->setNameSent(true);
				} else if(!blob->isNameAcknowleged()) {
					blob->setNameAcknowleged(true);
				}
			}
			//Acknowlege chunk send with a set % probability
			const int ackChance=(utility::random::qrand()%100);
			//qDebug()<<"ackChance="<<ackChance;
			if(ackChance>60) {
				chunk=blob->chunk(utility::random::qrand()%blob->numTotal());
				//qDebug()<<"IN ACK WITH "<<chunk.id() << " SENT="<<chunk.isSent() << " ACKED="<<chunk.isAcknowleged();

				if(chunk.isSent() && !chunk.isAcknowleged() ) {
					//qDebug()<<"CHUNK "<<chunk.id()<<" IS BEING ACKNOWLEGED";
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
	PixViewerWidget pixA;
	PixViewerWidget pixB;
	int imageSize;
	QImage rimgA;
	QByteArray ridA;
	const QString blobName;

public:
	explicit CourierTesterBlob(QSharedPointer<CommsChannel> comms)
		: CourierTester(OC_NEW BlobCourier(comms),OC_NEW BlobCourier(comms), "FROM", "TO  ")
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
		rimgA=utility::graphics::randomImage(imageSize, imageSize);
		ridA=utility::graphics::imageToByteArray(rimgA);
		pixA.setImage(rimgA);
		CourierMandate beforeMandate=mFromCourier->mandate();
		//qDebug()<<"before="<<beforeMandate.toString();
		BlobFuture ret=((BlobCourier *)mFromCourier)->submitSendingBlob(blobName,ridA,0.5);
		QVERIFY(ret.isWinning());
		QVERIFY(((BlobCourier *)mFromCourier)->totalSendingDataSize()>0);
		//qDebug()<<"TOTAL SEND IS " << ((BlobCourier *)mFromCourier)->totalSendingDataSize();

		QObject::connect(((BlobCourier *)mToCourier), &BlobCourier::blobReceiveComplete, this, [this](QString name)
		{
			//qDebug()<<"TEST DONE for "<<name;
			mDone=true;
		});

	}


	void onTestDeInitImp() Q_DECL_OVERRIDE {
		//qDebug()<<"TEST DONE, waiting for final show-off...";
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
			auto bytes=img.sizeInBytes();
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
	QSharedPointer<AddressBook> peers(OC_NEW AddressBook());
	peers->configure("test_blob_address_book.json");
	QSharedPointer<KeyStore> keystore(OC_NEW KeyStore());
	keystore->configure("test_blob_keystore.json");
	//CommsCarrier &carrier, KeyStore &keystore, NodeAssociateStore &peers, QObject *parent=nullptr);
	QSharedPointer<CommsCarrierUDP> carrier(OC_NEW CommsCarrierUDP());
	carrier->configure();
	QSharedPointer<CommsChannel> comms(OC_NEW CommsChannel());
	comms->configure(carrier, keystore, peers);
	CourierTesterBlob blobTest(comms);
	blobTest.testRandomSteps(100);
}


OC_TEST_MAIN(test, TestBlob)
