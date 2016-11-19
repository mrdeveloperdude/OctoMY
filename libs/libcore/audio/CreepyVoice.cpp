#include "CreepyVoice.hpp"


#include "../libutil/utility/Utility.hpp"

#include "random/RNG.hpp"

#include "security/PortableID.hpp"

#include <QDebug>


struct CreepyBuffer {
	int mSize;
	short *mBuffer;

	CreepyBuffer(int size)
		: mSize(size)
		, mBuffer(new short[mSize])
	{

	}

	~CreepyBuffer()
	{
		delete[] mBuffer;
		mSize=0;
	}

};



CreepyVoice::CreepyVoice(PortableID &id, QObject *parent)
	: QObject(parent)
	, mHz(0)
	, mInited(false)
	, bac(0)
	, cur(nullptr)
	, periodCounter(0)
	, comp()
	, reverb()
	, mID(id)
	, rng(RNG::sourceFactory("mt"))

{
}

CreepyVoice::~CreepyVoice()
{
	cur=nullptr;
	for(CreepyBuffer *buffer:mBuffersFree) {
		delete buffer;
	}
	mBuffersFree.clear();
	for(CreepyBuffer *buffer:mBuffersInUse) {
		delete buffer;
	}
	mBuffersInUse.clear();
}



double CreepyVoice::frandGauss()
{
	return rng->generateGauss();
}




double CreepyVoice::frandGaussAbs()
{
	return qAbs(rng->generateGauss());
}

void CreepyVoice::freeBuffer(CreepyBuffer *buf)
{
	if(nullptr!=buf) {
		mBuffersInUse.removeAll(buf);
		mBuffersFree.append(buf);
	}
}

CreepyBuffer *CreepyVoice::getFreeBuffer(int numsamples)
{
	CreepyBuffer *ret=nullptr;
	int i=0;
	for(CreepyBuffer *buffer:mBuffersFree) {
		if(buffer->mSize==numsamples) {
			CreepyBuffer *buffer=mBuffersFree.takeAt(i);
			if(nullptr!=buffer) {
				ret=buffer;
				break;
			}
		}
		i++;
	}
	if(nullptr==ret) {
		ret=new CreepyBuffer(numsamples);
	}
	if(nullptr!=ret) {
		mBuffersInUse.append(ret);
	}
	return ret;
}

#ifdef EXTERNAL_LIB_ESPEAK
void CreepyVoice::feed(short *wav, int numsamples, espeak_EVENT *events)
{
	const int numsamples2 = (numsamples * af.sampleRate()) / (mHz == 0 ? 1 : mHz);
	CreepyBuffer *buffer=getFreeBuffer(numsamples2);
	if(nullptr!=buffer) {
		short * dp = buffer->mBuffer;
		if (0 != dp) {
			for (int i = 0; i < numsamples2; i++) {
				const int j = (i * numsamples) / numsamples2;
				dp[i] = wav[j];
			}
		}
	}
}
#endif

bool CreepyVoice::isInitialized()
{
	return mInited;
}

void CreepyVoice::speak(QString word)
{
	if (isInitialized()) {
		qDebug() << " speaking \"" << word << "\"\n";
#ifdef EXTERNAL_LIB_ESPEAK
		espeak_Synth(word.toStdString().c_str(), word.size() + 1, 0, POS_CHARACTER, 0, espeakCHARS_AUTO, nullptr, (void *)this);
		//espeak_Synchronize();
#endif
	}
}

void CreepyVoice::deinit()
{
	if (isInitialized()) {
#ifdef EXTERNAL_LIB_ESPEAK
		espeak_Terminate();
#endif
	}
}



#ifdef EXTERNAL_LIB_ESPEAK
static int synthCallback(short *wav, int numsamples, espeak_EVENT *events)
{
	if (0 != wav) {
		CreepyVoice *es = static_cast<CreepyVoice *>(events->user_data);
		if(nullptr!=es) {
			es->feed(wav, numsamples, events);
		} else {
			qWarning()<<"ERROR: no espeak instance in synth callback";
		}
	}
	return 0;
}
#endif



//IAudioSource interface

void CreepyVoice::init(QAudioFormat f)
{
	if (isInitialized()) {
		return;
	}
	af=f;
	QString seed=(mID.id()+"OctoMY™ Voice");
	rng->init(seed.toUtf8());
	qDebug()<<"INITIALIZING VOICE WITH SEED: "<<seed;

#ifdef EXTERNAL_LIB_ESPEAK
	mHz = espeak_Initialize(AUDIO_OUTPUT_RETRIEVAL, 10, 0, 0);
	if(mHz >0) {
		espeak_SetSynthCallback(synthCallback);
		espeak_SetVoiceByName("en");

		/*
		 * cool
		espeak_SetParameter(espeakRATE, 100, 0);
		espeak_SetParameter(espeakPITCH, 20, 0);
		espeak_SetParameter(espeakVOLUME, 100, 0);
		espeak_SetParameter(espeakRANGE, 0, 0);
		*/
		espeak_SetParameter(espeakRATE, (int)(frandGauss()*50.0+90), 0);
		espeak_SetParameter(espeakPITCH, (int)(frandGauss()*40.0+50), 0);
		espeak_SetParameter(espeakVOLUME, 150, 0);
		espeak_SetParameter(espeakRANGE, 0, 0);
		espeak_VOICE *voice_spec = espeak_GetCurrentVoice();
		voice_spec->gender = "Male"==mID.gender()?1:0; //male
		voice_spec->age = 32; //age in years
		espeak_SetVoiceByProperties(voice_spec);
	} else {
		qWarning()<<"ERROR: Could not initialize espeak";
	}

#endif

	tremolo.init(af);
	tremolo.set(frandGaussAbs()*25.0, 0.00001+frandGaussAbs()*0.00018);

	comp.set(0.5+frandGauss()*0.2, 0.5+frandGauss()*0.2 , (3.0+frandGauss()*0.2)/1000.0 , (1.0+frandGauss()*0.2)/1000.0 , (0.1+frandGauss()*0.2)/1000.0 , (300.0+frandGauss()*50.0)/1000.0);
	comp.init(af);
	int rsr=af.sampleRate()*OUTPUT_CHANNELS;
	reverb.init(rsr);;
	reverb.
	//Castle

	LoadPreset(25.0f+frandGauss()*100,
			   8.3f+frandGauss()*20.0,
			   0.890f+frandGauss()*0.3,
			   -1000.0f+frandGauss()*100.0,
			   -200.0f-frandGauss()*100.0,
			   -2000.0f+frandGauss()*600.0,
			   1.22f+frandGauss()*0.2,
			   0.83f+frandGauss()*0.1,
			   0.31f+frandGauss()*0.1,
			   -100.0f-frandGauss()*10.0,
			   0.022f+frandGauss()*0.002,
			   0.0f+frandGaussAbs(),
			   0.0f+frandGaussAbs(),
			   0.0f+frandGaussAbs(),
			   600.0f+frandGauss()*100.0,
			   0.011f+frandGauss()*0.001,
			   0.0f+frandGaussAbs(),
			   0.0f+frandGaussAbs(),
			   0.0f+frandGaussAbs(),
			   0.138f+frandGauss()*0.01,
			   0.080f+frandGauss()*0.01,
			   0.250f+frandGauss()*0.1,
			   0.0f+frandGaussAbs(),
			   -5.0f-frandGauss()*0.1,
			   5168.6f+frandGauss()*500,
			   139.5f+frandGauss()*13.0,
			   0.0f+frandGaussAbs(), 0x20);

	//LoadPreset(26.0f, 8.3f, 0.890f, -1000.0f, -800.0f, -2000.0f, 1.22f, 0.83f, 0.31f, -100.0f, 0.022f, 0.0f, 0.0f, 0.0f, 600.0f, 0.011f, 0.0f, 0.0f, 0.0f, 0.138f, 0.080f, 0.250f, 0.0f, -5.0f, 5168.6f, 139.5f, 0.0f, 0x20);
	//stadium
	//LoadPreset(26, 7.2f, 1.0f, -1000, -700, -200, 6.26f, 0.51f, 1.10f, -2400, 0.183f, 0.0f, 0.0f, 0.0f, -800, 0.038f, 0.0f, 0.0f, 0.0f, 0.250f, 0.0f, 0.250f, 0.0f, -5.0f, 5000.0f, 250.0f, 0.0f, 0x20);
	// heaven
	//LoadPreset(26, 19.6f, 0.940f, -1000, -200, -700, 5.04f, 1.12f, 0.56f, -1230, 0.020f, 0.0f, 0.0f, 0.0f, 200, 0.029f, 0.0f, 0.0f, 0.0f, 0.250f, 0.080f, 2.742f, 0.050f, -2.0f, 5000.0f, 250.0f, 0.0f, 0x3f);
	//sewer
	//LoadPreset(21, 1.7f, 0.800f, -1000, -1000, 0, 2.81f, 0.14f, 1.0f, 429, 0.014f, 0.0f, 0.0f, 0.0f, 1023, 0.021f, 0.0f, 0.0f, 0.0f, 0.250f, 0.0f, 0.250f, 0.0f, -5.0f, 5000.0f, 250.0f, 0.0f, 0x3f);
//Crazy
//		LoadPreset(25, 1.0f, 0.500f, -1000, -151, 0, 7.56f, 0.91f, 1.0f, -626, 0.020f, 0.0f, 0.0f, 0.0f, 774, 0.030f, 0.0f, 0.0f, 0.0f, 0.250f, 0.0f, 4.0f, 1.0f, -5.0f, 5000.0f, 250.0f, 0.0f, 0x1f);
	reverb.Update(rsr);

	mInited=true;

}



void CreepyVoice::generate(qint64 num, double *out)
{
	if(nullptr!=out) {
		for(qint64 i=0; i<num; ++i) {
			if(nullptr==cur) {
				if(mBuffersInUse.count()>0) {
					cur=mBuffersInUse.takeFirst();
					bac=0;
				}
			}
			double sam=0.0;
			if(nullptr!=cur) {
				sam=cur->mBuffer[bac];
				sam/=(double)0xFFFF;

				bac++;
				if(bac>=cur->mSize) {
					freeBuffer(cur);
					cur=nullptr;
				}
				periodCounter=0;
			} else {
				//Leave 3 buffers worth of pause at the end to make sure the audio output system gets throug all the buffers before we shut it down.
				const int LIMIT=3*num;
				if(periodCounter<LIMIT) {
					//qDebug()<<"Countind down " <<periodCounter;
					periodCounter++;
				}
				if(periodCounter==LIMIT) {
					periodCounter++;
					emit periodComplete();
				}
				sam=utility::frand()*0.02;
			}

			if (sam > 0) {
				sam = 1 - exp(-sam*distAmount);
			} else {
				sam = -1 + exp(sam*distAmount);
			}


			out[i]=sam;
		}

		reverb.Process(num,out,floatSamplesOut);
		for(int i=0; i<num; ++i) {
			out[i]=(out[i]+floatSamplesOut[i])*0.8;
		}

		tremolo.process(num,out,floatSamplesOut);


		comp.process(num,floatSamplesOut,out);
	}

}
