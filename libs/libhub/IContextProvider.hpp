#ifndef ICONTEXTPROVIDER_HPP
#define ICONTEXTPROVIDER_HPP

#include "basic/Settings.hpp"

class IContextProvider
{
public:

	virtual Settings &settings()=0;
};

#endif // ICONTEXTPROVIDER_HPP
