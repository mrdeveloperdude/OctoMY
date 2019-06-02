#ifndef TASKQUEUE_HPP
#define TASKQUEUE_HPP

#include "ConcurrentQueue.hpp"

//typedef std::function<void()> TaskType;

template <typename T>
class TaskQueue : public ConcurrentQueue<T>
{
public:
	TaskQueue(int capacity=0);

public:

	bool runOne();
	void runAll();

};


template <typename T>
TaskQueue<T>::TaskQueue(int capacity)
	: ConcurrentQueue<T>(capacity)
{
	OC_METHODGATE();
}

template <typename T>
bool TaskQueue<T>::runOne()
{
	OC_METHODGATE();
	bool got=false;
	T task=ConcurrentQueue<T>::tryGet(got);
	if(got){
		task();
	}
	return got;
}

template <typename T>
void TaskQueue<T>::runAll()
{
	OC_METHODGATE();
	while(runOne());
}


#endif
// TASKQUEUE_HPP
