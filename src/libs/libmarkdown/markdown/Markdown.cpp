#include "Markdown.hpp"

#ifdef USE_MARKDOWN



Markdown::Markdown()
{

}

bool ZBarScanner::isAvailable()
{
	return true;
}



#else






// No-op
Markdown::Markdown()
{
}

bool Markdown::isAvailable()
{
	return false;
}



#endif
