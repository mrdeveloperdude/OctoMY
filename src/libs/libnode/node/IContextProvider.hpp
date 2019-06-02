#ifndef ICONTEXTPROVIDER_HPP
#define ICONTEXTPROVIDER_HPP

#include "app/Settings.hpp"

class IContextProvider
{
public:

	virtual Settings &settings()=0;

	virtual ~IContextProvider();
};

#endif // ICONTEXTPROVIDER_HPP
