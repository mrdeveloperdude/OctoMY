#ifndef UTILITY_CONCURRENT_HPP
#define UTILITY_CONCURRENT_HPP


#include "uptime/New.hpp"

#include <QCoreApplication>

#include <QRecursiveMutex>
#include <QDebug>
#include <QAbstractEventDispatcher>

namespace utility
{

namespace concurrent
{

// FROM https://stackoverflow.com/questions/21646467/how-to-execute-a-functor-or-a-lambda-in-a-given-thread-in-qt-gcd-style/21653558#21653558
template <typename F>
struct FunctorEvent : public QEvent {
	using Functor = typename std::decay<F>::type;
	Functor functor;
	explicit FunctorEvent(Functor && fun) : QEvent(QEvent::None), functor(std::move(fun)) {}
	explicit FunctorEvent(const Functor & fun) : QEvent(QEvent::None), functor(fun) {}
	virtual ~FunctorEvent()
	{
		//qDebug()<<"CALLING FUNCTOR FOR FUNCTOR EVENT";
		functor();
		//qDebug()<<"DONE CALLING FUNCTOR FOR FUNCTOR EVENT";
	}
};


template <typename F>
void postToObject(F && functor, QObject * obj = qApp)
{
	if (qobject_cast<QThread*>(obj)) {
		qWarning() << "posting a call to a thread object - consider using postToThread";
	}
	QCoreApplication::postEvent(obj, OC_NEW FunctorEvent<F>(std::forward<F>(functor)));
}


template <typename F>
void postToThread(F && functor, QThread * thread = qApp->thread())
{
	QObject * obj = QAbstractEventDispatcher::instance(thread);
	Q_ASSERT(obj);
	QCoreApplication::postEvent(obj, OC_NEW FunctorEvent<F>(std::forward<F>(functor)));
}


class Q_CORE_EXPORT MutexTryLocker
{
private:
	Q_DISABLE_COPY(MutexTryLocker)
	quintptr val;

public:
	inline explicit MutexTryLocker(QBasicMutex *m) QT_MUTEX_LOCK_NOEXCEPT {
		Q_ASSERT_X((reinterpret_cast<quintptr>(m) & quintptr(1u)) == quintptr(0), "MutexTryLocker", "QMutex pointer is misaligned");
		val = quintptr(m);
		if (Q_LIKELY(m))
		{
			// call QMutex::lock() instead of QBasicMutex::lock()
			const bool is_locked=static_cast<QMutex *>(m)->try_lock();
			if(is_locked) {
				val |= 1;
			}
		}
	}


	inline ~MutexTryLocker()
	{
		unlock();
	}


	inline void unlock() Q_DECL_NOTHROW {
		if ((val & quintptr(1u)) == quintptr(1u))
		{
			val &= ~quintptr(1u);
			mutex()->unlock();
		}
	}


	inline void relock() QT_MUTEX_LOCK_NOEXCEPT {
		if (val)
		{
			if ((val & quintptr(1u)) == quintptr(0u)) {
				mutex()->lock();
				val |= quintptr(1u);
			}
		}
	}


	inline QRecursiveMutex *mutex() const
	{
		return reinterpret_cast<QRecursiveMutex *>(val & ~quintptr(1u));
	}

};


class HandleCounter
{
private:
	quint64 handleCounter;
	QMap<Qt::HANDLE, quint64> handleMap;
	QRecursiveMutex handleMutex;

public:

	HandleCounter();
	QString handleCounterString(Qt::HANDLE h);

};

QString handleCounterString(Qt::HANDLE h);
QString currentThreadID();


}
}

#endif
// UTILITY_CONCURRENT_HPP
