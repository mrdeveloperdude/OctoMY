#ifndef TRYVALUE_HPP
#define TRYVALUE_HPP

#include <functional>
#include <vector>

template <typename T>
class TryValue
{
private:
	T mTarget;
	T mActual;
	bool mAcknowledged;
	
	std::vector<std::function<void(const T&)>> mTargetChangedCallbacks;
	std::vector<std::function<void(const T&)>> mActualChangedCallbacks;
	std::vector<std::function<void()>> mValuesAcknowledgedCallbacks;

public:
	explicit TryValue();

public:
	void setTarget(const T &newValue);
	void updateActual(const T &newActual);

public:
	inline const T &getTarget() const { return mTarget; }
	inline const T &getActual() const { return mActual; }
	inline bool isAcknowledged() const { return mAcknowledged; }

	void connectTargetChanged(std::function<void(const T&)> callback);
	void connectActualChanged(std::function<void(const T&)> callback);
	void connectValuesAcknowledged(std::function<void()> callback);

private:
	void emitTargetChanged(const T &value);
	void emitActualChanged(const T &value);
	void emitValuesAcknowledged();

};




template <typename T>
TryValue<T>::TryValue()
	: mAcknowledged(false)
{
	
}

template <typename T>
void TryValue<T>::setTarget(const T &newValue) {
	if (newValue != mTarget) {
		mTarget = newValue;
		mAcknowledged = false;
		emitTargetChanged(newValue);
	}
}

template <typename T>
void TryValue<T>::updateActual(const T &newActual) {
	mActual = newActual;
	mAcknowledged = true;
	mTarget = mActual; // Set mTarget to match mActual when acknowledged
	emitActualChanged(newActual);
	emitValuesAcknowledged();
}

template <typename T>
void TryValue<T>::connectTargetChanged(std::function<void(const T&)> callback) {
	mTargetChangedCallbacks.push_back(callback);
}

template <typename T>
void TryValue<T>::connectActualChanged(std::function<void(const T&)> callback) {
	mActualChangedCallbacks.push_back(callback);
}

template <typename T>
void TryValue<T>::connectValuesAcknowledged(std::function<void()> callback) {
	mValuesAcknowledgedCallbacks.push_back(callback);
}

template <typename T>
void TryValue<T>::emitTargetChanged(const T &value) {
	for (auto &callback : mTargetChangedCallbacks) {
		callback(value);
	}
}

template <typename T>
void TryValue<T>::emitActualChanged(const T &value) {
	for (auto &callback : mActualChangedCallbacks) {
		callback(value);
	}
}

template <typename T>
void TryValue<T>::emitValuesAcknowledged() {
	for (auto &callback : mValuesAcknowledgedCallbacks) {
		callback();
	}
}



#endif // TRYVALUE_HPP
