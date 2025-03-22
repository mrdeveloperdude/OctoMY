#include "Action.hpp"

#include "utility/random/Random.hpp"


#include <QFile>
#include <QDebug>


ActionBase::ActionBase(const QString &phraseText)
	: mID{utility::random::randomString(10)}
	, mPhrase(QSharedPointer<ParameterPhrase>::create(phraseText))
{
}
