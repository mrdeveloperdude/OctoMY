#ifndef ATOMICBOOLEAN_HPP
#define ATOMICBOOLEAN_HPP

#include <QMutex>

class AtomicBoolean{
	private:
		QMutex mMutex;
		bool mBool;
		Q_DISABLE_COPY(AtomicBoolean)

	public:
		AtomicBoolean(bool v)
			:
			  mMutex()
			, mBool(false)
		{
			set(v);
		}

		bool set(bool v)
		{
			QMutexLocker ml(&mMutex);
			return mBool=v;
		}

		bool get()
		{
			QMutexLocker ml(&mMutex);
			return mBool;
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

#endif // ATOMICBOOLEAN_HPP
