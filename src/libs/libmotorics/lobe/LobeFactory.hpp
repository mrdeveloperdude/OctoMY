#ifndef LobeFACTORY_HPP
#define LobeFACTORY_HPP

#include "uptime/SharedPointerWrapper.hpp"

#include <QString>

class Lobe;

class LobeFactory
{
public:
	LobeFactory();

public:
	QSharedPointer<Lobe> createLobe(const QString &type);
	QStringList availableLobes();
};

#endif
// LobeFACTORY_HPP
