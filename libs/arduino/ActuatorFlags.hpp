#ifndef ACTUATORFLAGS_HPP
#define ACTUATORFLAGS_HPP

#define ACTUATOR_FLAG_SELECTOR(NAME_GET, NAME_SET, BIT)  \
	inline bool NAME_GET() const { return ( flags & (1<<(BIT)) ) > 0; } \
	inline void NAME_SET(bool val) { if(val) { flags=flags|(1<<(BIT)); } else { flags=flags&(~(1<<(BIT))); } }


#endif // ACTUATORFLAGS_HPP
