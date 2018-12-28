#ifndef ATOMICBOOLEAN_HPP
#define ATOMICBOOLEAN_HPP

#include <QMutex>
#include <QAtomicInteger>
#include <QDebug>

class AtomicBoolean{
	private:
		//QMutex mMutex;
		//bool mBool;
		QAtomicInteger<int> mBoolInt;
		Q_DISABLE_COPY(AtomicBoolean)

	public:
		AtomicBoolean(bool v)
			:
			//  mMutex()
			//, mBool(false)
			 mBoolInt(0)
		{
			set(v);
		}

		bool set(bool v)
		{
			//qDebug()<<"atomic bool transition from "<<(0!=mBoolInt)<<" -> "<< v;
			//QMutexLocker ml(&mMutex);
			mBoolInt=(v?1:0);
			return true;
			//return (mBool=v);
		}

		bool get()
		{
			//QMutexLocker ml(&mMutex);
			const bool ret=(0!=mBoolInt);
			return ret;
			//return mBool;
		}


		void operator=(const bool &b)
		{
			set(b);
		}

		operator bool()
		{
			return get();
		}

};

#endif
// ATOMICBOOLEAN_HPP
