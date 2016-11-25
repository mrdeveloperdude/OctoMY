#include "BlobCourier.hpp"

#include "Blob.hpp"
#include "BlobChunk.hpp"


#include <QtGlobal>
#include <QDebug>


const quint16 BlobCourier::BLOB_CHUNK_SIZE=400;


const QDebug &operator<<(QDebug &d, const BlobCourier::Ack &a)
{
	d.nospace().noquote()<<"ack.blobID="<<a.blobID()<< (a.isNameAck()?", ack.isNameAck=TRUE":(", ack.chunkID="+QString::number(a.chunkID())));
	return d.space();
}



const QDataStream &operator<<(QDataStream &d, const BlobCourier::Ack &a)
{
	d<<a.mBlobID;
	d<<a.mChunkID;
	return d;

}


const QDataStream &operator>>(QDataStream &d, BlobCourier::Ack &a)
{
	quint16 b=0;
	d>>b;
	a.mBlobID=b;
	quint16 c=0;
	d>>c;
	a.mChunkID=c;
	return d;
}


BlobCourier::BlobCourier(QObject *parent)
	: Courier("Blob",(Courier::FIRST_USER_ID+4), parent)
	, mTotalSendingBlobDataSize(0)
	, mTotalReceivingBlobDataSize(0)
{

}

quint16 BlobCourier::findFreeSendingBlobID()
{
	quint16 out=0;
	const quint16 M=0xFFFF;
	do {
		out=qrand()%M;
	} while((0 == out) || (mSendingBlobsById.contains(out)) );
	return out;
}


quint16 BlobCourier::findNextSendingBlobID()
{
	quint16 out=0;
	//Avoid FPE
	if(mTotalSendingBlobDataSize<=0) {
		//qWarning()<<"WARNING: Next blob was 0 due to mTotalSendingBlobDataSize <=0 "<<mTotalSendingBlobDataSize;
		return out;
	}
	// Select a random spot
	quint64 spot=(qrand()%mTotalSendingBlobDataSize);
	quint64 acc=0;
	// Accumulate size until accumulator exceeds said spot
	for(QHash<quint16, SendingBlob *>::iterator it=mSendingBlobsById.begin(), e=mSendingBlobsById.end(); it!=e; ++it) {
		acc+=it.value()->dataSize();
		out=it.key();
		if(acc>spot) {
			//qDebug()<<"random blob selection ended with "<<spot<<acc;
			//Return the key of that blob to ensure evenly distributed chunk selection
			break;
		}
	}
	return out;
}

BlobFuture BlobCourier::submitSendingBlob(QString name, QByteArray data, qreal probability)
{
	static const quint64 MAX_BLOB_SIZE=2LL*1024*1024*1024;
	BlobFuture future(name, this);
	if(name.size()<=0) {
		future.fail("Name too long");
	} else if(name.size()>10) {
		future.fail("Name too short");
	} else if(mSendingBlobsByName.contains(name)) {
		future.fail("Blob name allready submitted");
	} else if(mSendingBlobsById.size()>64000) {
		future.fail("Too many simultaneous blobs");
	} else {
		const quint16 id=findFreeSendingBlobID();
		if(0==id) {
			future.fail("Could not generate blob ID");
		} else {
			const quint64 newTotalBlobSize=mTotalSendingBlobDataSize+data.size();
			// TODO: make this limit adapt to available application memory (or config limitation)
			if(newTotalBlobSize>=MAX_BLOB_SIZE) {
				future.fail("Too much data in transit");
			} else {
				SendingBlob *blob=new SendingBlob(name, id, BLOB_CHUNK_SIZE, data, qBound(0.1,probability,0.9));
				if(nullptr==blob) {
					future.fail("Could not allocate blob");
				}
				else {
					mSendingBlobsById.insert(id,blob);
					mSendingBlobsByName.insert(name,blob);
					mTotalSendingBlobDataSize=newTotalBlobSize;
					future.win();
				}
			}
		}
	}
	//Done
	return future;
}



QByteArray BlobCourier::dataForSendingBlob(QString name)
{
	if(mSendingBlobsByName.contains(name)) {
		Blob *blob=mSendingBlobsByName[name];
		if(nullptr!=blob) {
			return blob->data();
		}
	}
	return QByteArray();
}


QByteArray BlobCourier::dataForReceivingBlob(QString name)
{
	if(mReceivingBlobsByName.contains(name)) {
		Blob *blob=mReceivingBlobsByName[name];
		if(nullptr!=blob) {
			return blob->data();
		}
	}
	return QByteArray();
}


void BlobCourier::freeSendingBlob(QString name)
{
	qWarning()<<"ERROR: NOT IMPLEMENTED YET! ";
}

quint64 BlobCourier::totalSendingDataSize()
{
	return mTotalSendingBlobDataSize;
}

quint64 BlobCourier::totalReceivingDataSize()
{
	return mTotalReceivingBlobDataSize;
}


void BlobCourier::printSendingSummary(QString title)
{
	int tot=0;
	int sent=0;
	int acked=0;
	qDebug()<<"------------------------------------------------------ "<<title <<" SENDING BLOBS:";
	QList<SendingBlob *> sendingBlobs=mSendingBlobsById.values();
	for(SendingBlob *blob:sendingBlobs) {
		if(nullptr!=blob) {
			const int back=blob->numAcknowleged();
			const int bsend=blob->numSent();
			const int btot=blob->numTotal();
			acked+=back;
			sent+=bsend;
			tot+=btot;
			qDebug()<<" + sending blob "<<(blob->isDone()?"DONE":"----")<<" "<<blob->name()<<"("<<blob->id()<<")"<<(blob->isNameAcknowleged()?"[ACK]":(blob->isNameSent()?"[SEND]":"[NONE]") )<<" total="<<btot<<",  sent="<<bsend<<",  acked="<<back<<", transit="<<blob->isSent()<<", ack="<<blob->isAcknowleged();
		} else {
			qDebug()<<" + sending blob NULL";
		}
	}
	qDebug()<<"SENDING SUMMARY: blobs="<<sendingBlobs.size()<<" total="<<tot<<",  sent="<<sent<<",  acked="<<acked<<", "<<",  totalSendBytes="<<mTotalSendingBlobDataSize;
	qDebug()<<"------------------------------------------------------ "<<title <<" RECEIVING BLOBS:";
	tot=0;
	sent=0;
	acked=0;
	QList<ReceivingBlob *> receivingBlobs=mReceivingBlobsById.values();
	for(ReceivingBlob *blob:receivingBlobs) {
		if(nullptr!=blob) {
			const int brec=blob->numReceived();
			const int btot=blob->numTotal();
			sent+=brec;
			tot+=btot;
			qDebug()<<" + receiving blob "<<(blob->isDone()?"DONE":"----")<<" "<<blob->name()<<"("<<blob->id()<<")"<<(blob->isNameReceived()?"[REC]":"[NONE]")<<" total="<<btot<<",  received="<<brec<<", transit="<<blob->isReceived();
		} else {
			qDebug()<<" + receiving blob NULL";
		}
	}

	qDebug()<<"RECEIVING SUMMARY: blobs="<<receivingBlobs.size()<<" total="<<tot<<",  sent="<<sent<<",  acked="<<acked<<", "<<",  totalReceiveBytes="<<mTotalReceivingBlobDataSize;
}


////////////////// Courier interface

CourierMandate BlobCourier::mandate()
{
	const bool hasUnsentBlobs=(!mSendingBlobsById.isEmpty()) || (!mReceivingBlobsById.isEmpty());
	return CourierMandate(BLOB_CHUNK_SIZE,1,100, hasUnsentBlobs);
}

quint16 BlobCourier::sendingOpportunity(QDataStream &ds)
{
	quint16 bytes=0;
	// Get blob to send if any
	const quint16 blobID=findNextSendingBlobID();

	// Send Blob ID.
	// NOTE: invalid blob ID (0) simply means skip blob/chunk/name and do pure acks
	ds << blobID;
	bytes+=sizeof(quint16);
	//qDebug()<<"BLOB TX blobID="<<blobID;

	// Look up blob by ID
	SendingBlob *blob=nullptr;
	if(0!=blobID) {
		blob=mSendingBlobsById[blobID];
		if(nullptr==blob) {
			qWarning()<<"ERROR: blob ID error for "<<blobID;
		}
	}
	if(nullptr!=blob) {
		if(!blob->isDone()) {
			// Send Blob Name and Size in all packets until it is acknowleged
			if(!blob->isNameAcknowleged()) {
				const QString name=blob->name().left(10);
				const QByteArray nameArray=name.toUtf8();
				const quint8 nameSize=nameArray.size();
				// Send Blob Name Size
				ds << nameSize;
				bytes+=sizeof(quint8);
				//qDebug()<<"BLOB TX nameSize="<<nameSize;

				// Send Blob Name Characters
				for(quint8 i=0; i<nameSize; ++i) {
					const quint8 ch=nameArray.at(i);
					ds<<ch;
					bytes+=sizeof(quint8);
				}
				// Send Blob Size
				const quint64 blobSize=blob->dataSize();
				ds << blobSize;
				bytes+=sizeof(quint64 );
				//qDebug()<<"BLOB TX blobSize="<<blobSize;

				// Mark blob name as sent
				blob->setNameSent(true);
				//qDebug()<<"BLOB TX SENT NAME & SIZE: '"<<name<<"', "<<blobSize;
			} else {
				// Send Name Size of 0 to indicate that no Name characters or Blob Size will be part of this packet
				const quint8 nameSize=0;
				ds << nameSize;
				bytes+=sizeof(quint8);
				//qDebug()<<"BLOB TX nameSize="<<nameSize;
			}
			SendingBlobChunk chunk = blob->findNextSendingChunk();
			if(chunk.isValid()) {
				// Send Chunk ID
				const quint32 chunkID=chunk.index();
				ds << chunkID;
				bytes+=sizeof(quint16);
				//qDebug()<<"BLOB TX chunkID="<<chunkID;

				// Send Chunk Size
				QByteArray data=chunk.data();
				const quint16 chunkSize=(quint16)data.size();
				ds << chunkSize;
				bytes+=sizeof(quint16);
				//qDebug()<<"BLOB TX chunkSize="<<chunkSize;

				// Send Chunk Checksum
				const quint16 chunkChecksumCalc=qChecksum(data.data(),chunkSize);
				ds << chunkChecksumCalc;
				bytes+=sizeof(quint16);
				//qDebug()<<"BLOB TX chunkChecksumCalc="<<chunkChecksumCalc;

				// Send chunk data
				ds.writeRawData(data.data(),chunkSize);
				bytes+=chunkSize;
				//qDebug()<<"BLOB TX writeRawData";
				// Put some log stats
				//qDebug()<<"BLOB TX " << chunkSize << " bytes of chunk "<<chunk.id()<< " IS BEING "<<(chunk.isSent()?"RESENT":"SENT") << " with blobID="<<blobID<< ", chunkID="<<chunkID<< ", chunkChecksumCalc="<<chunkChecksumCalc;
				// Mark chunk as sent
				chunk.setSent();
			} else {
				// Send Empty Chunk ID to signal no chunk data in this packet
				const quint32 chunkID=0;
				ds << chunkID;
				bytes+=sizeof(quint16);
				//qDebug()<<"BLOB TX chunkID="<<chunkID;
			}
		} else {
			qWarning()<<"WARNING: Found completed blob in list";
		}
	} else {
		//qDebug()<<"Blobless packet";
	}
	// Send Ack Count
	const quint16 MAX_ACKS=(BLOB_CHUNK_SIZE-bytes-4+8)/4;
	const quint16 numAcks=mPendingAcks.size();
	const quint8 ackCount=qMin(numAcks,MAX_ACKS);
	ds << ackCount;
	bytes+=sizeof(quint8);
	//qDebug()<<"BLOB TX ackCount="<<ackCount<<numAcks<<MAX_ACKS;
	// Send Acks
	for(int i=0; i < ackCount; ++i) {
		Ack *ack=mPendingAcks.takeAt(0);
		ds<<*ack;
		bytes+=ack->bytes();
		delete ack;
		//qDebug()<<"BLOB TX  + ack="<<*ack;

	}

	//Return number of bytes sent
	return bytes;
}


quint16 BlobCourier::dataReceived(QDataStream &ds, quint16 availableBytes)
{
	quint16 bytes=0;

	// Read blob ID
	quint16 blobId=0;
	ds >> blobId;
	bytes+=sizeof(quint16);
	//qDebug()<<"BLOB RX blobId="<<blobId;

	// NOTE: invalid blob ID (0) simply means skip blob/chunk/name and do pure acks
	if(0!=blobId) {
		// Read Name Size
		quint8 nameSize=0;
		ds >> nameSize;
		bytes+=sizeof(quint8);
		//qDebug()<<"BLOB RX nameSize="<<nameSize;

		ReceivingBlob *blob=nullptr;

		// Read Name Characters and Blob Size only when Name Size is greater than 0
		if(nameSize>0) {

			// Read Name Characters
			QByteArray nameArray;
			for(quint8 i=0; i<nameSize; ++i) {
				quint8 ch=0;
				ds  >> ch;
				bytes+=sizeof(quint8);
				nameArray[i]=ch;
			}

			// Read Blob Size
			quint64 blobSize=0;
			ds >> blobSize;
			bytes+=sizeof(quint64);

			// Construct Name string from Name Characters
			const QString name=QString::fromUtf8(nameArray.data(),nameArray.size());
			//qDebug()<<"BLOB RX name="<<name<<" blobSize="<<blobSize;

			// Register new blob with newly received name and size
			if(!mReceivingBlobsByName.contains(name)) {
				blob=new ReceivingBlob(name, blobId, BLOB_CHUNK_SIZE, blobSize );
				mReceivingBlobsByName.insert(name, blob);
				mReceivingBlobsById.insert(blobId, blob);
				//qDebug()<<"BLOB RX new rx blob registered: "<<name<<"="<<blobId;
				// Append pending ack for received name
				Ack *ack=new Ack(blobId);
				if(nullptr!=ack) {
					//qDebug()<<"BLOB RX appending pending ack for name";
					mPendingAcks.append(ack);
				}
			} else {
				blob=mReceivingBlobsById[blobId];
			}
			// Mark Blob Name as read
			if(nullptr!=blob) {
				blob->setNameReceived(true);
				//qDebug()<<"BLOB RX name received=true";
			}
		} else {
			// No name in transmission means the name ack was well received
			blob=mReceivingBlobsById[blobId];
			if(nullptr!=blob) {
				//qDebug()<<"BLOB RX name ack=true";
			}
		}

		// Read Chunk ID
		quint32 chunkId=0;
		ds >> chunkId;
		bytes+=sizeof(quint16);
		//qDebug()<<"BLOB RX chunkId="<<chunkId;

		// Read Chunk Size
		quint16 chunkSize=0;
		ds >> chunkSize;
		bytes+=sizeof(quint16);
		//qDebug()<<"BLOB RX chunkSize="<<chunkSize;

		// Read Chunk Cehcksum
		quint16 chunkChecksum=0;
		ds >> chunkChecksum;
		bytes+=sizeof(quint16);
		//qDebug()<<"BLOB RX chunkChecksum="<<chunkChecksum;

		// Read Chunk Data
		// NOTE: Due to the fact that our datatype is limited to positive integers below 64k we really don't need to check the value before creating array on stack
		char chunkBytesIn[chunkSize];
		ds.readRawData(chunkBytesIn,chunkSize);

		// Calculate Checksum of received data
		const quint16 chunkChecksumCalc=qChecksum(chunkBytesIn,chunkSize);
		//qDebug()<<"BLOB RX chunkChecksumCalc="<<chunkChecksumCalc;

		// Compare Checksums
		const bool checksumOK=(chunkChecksumCalc==chunkChecksum);
		//qDebug()<<"BLOB RX checksumOK="<<checksumOK;

		if(checksumOK) {
			if(nullptr!=blob) {
				ReceivingBlobChunk chunk=blob->chunk(chunkId);
				if(chunk.isValid()) {
					// Mark Chunk as read
					chunk.setReceived(chunkBytesIn, chunkSize);
					// Append pending ack for this chunk
					Ack *ack=new Ack(blobId, chunkId);
					if(nullptr!=ack) {
						//qDebug()<<"BLOB RX appending pending ack for chunk";
						mPendingAcks.append(ack);
					}
					// Mark Blob as complete
					if(blob->isDone()) {
						qDebug()<<"BLOB DONE!";
						emit blobReceiveComplete(blob->name());
						//TODO: Remove blob from courier at this point
					}
				} else {
					qWarning()<<"ERROR: could not find RX chunk by id: "<<chunkId;
				}
			} else {
				qWarning()<<"ERROR: RX blob was null";
			}
		} else {
			qWarning()<<"ERROR: received chunk checksum did not match data";
		}
	} else {
		//qDebug()<<"BLOB RX blobless packet";
	}

	// Read Ack Count
	quint8 ackCount=0;
	ds >> ackCount;
	bytes+=sizeof(quint8);
	//qDebug()<<"BLOB RX ackCount="<<ackCount;

	// Read Ack Data
	for(quint8 i=0; i<ackCount; ++i) {
		Ack ack(0);
		ds>>ack;
		bytes+=ack.bytes();
		//qDebug()<<"BLOB RX  + ack="<<ack;
		// Process Ack
		if(mSendingBlobsById.contains(ack.blobID())) {
			SendingBlob *ackBlob=mSendingBlobsById[ack.blobID()];
			if(nullptr!=ackBlob) {
				// Process Name Ack
				if(ack.isNameAck()) {
					// Mark Blob Name and Size as Acknowleged
					ackBlob->setNameAcknowleged(true);
					//qDebug()<<"BLOB RX name acknowleged for blob "<<ack.blobID();
				} else {
					SendingBlobChunk ackChunk=ackBlob->chunk(ack.chunkID());
					// Process Chunk Ack
					if(ackChunk.isValid()) {
						//qDebug()<<"BLOB RX chunk acknowleged="<<ackChunk.id();
						ackChunk.setAcknowleged();
					}
				}
			}
		}
	}

	// Return number of bytes read
	return bytes;
}
