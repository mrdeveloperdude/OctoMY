#ifndef METHODGATE_HPP
#define METHODGATE_HPP


#ifdef USE_FEATURE_BUFFER_HONEYPOT
#define BUFFER_HONEYPOT BufferHoneyPot bhp_(true)
#else
#define BUFFER_HONEYPOT
#endif

#define OC_LOWEST_POSSIBLE_ADDRESS (0xd0000)

#define OC_METHODGATE()  MethodGate ocMethodGate(this); (void)(ocMethodGate); BUFFER_HONEYPOT
#define OC_FUNCTIONGATE()  FunctionGate ocFunctionGate; (void)(ocFunctionGate); BUFFER_HONEYPOT



class MethodGate{

public:

	MethodGate(const void *parentThis);
};


class FunctionGate{

public:

	FunctionGate();
};

#endif
// METHODGATE_HPP
