#ifndef CLTOOL_HPP
#define CLTOOL_HPP

#include "CLInclude.hpp"


class QOpenGLContext;
/**
 * @brief The CLTool class is a tool class that contains a collection of all
 * openCL and context wrapper that gets called whenever OpenCL is used.
 *
 * The wrapper will in turn facilitate:
 *
 *  + Extra Convenience methods
 *  + Built-in error handling
 *  + Better testability
 *  + Isolatioon to OpenCL versions
 *
 *
 * Usage: create and keep an instance of this class as long as you are doing OpenCL. destruct when done.
 *
 */
class CLTool
{
	public:
		CLTool();

	public:

		cl_context_properties *generateProperties( const cl_platform_id platformID, QOpenGLContext *ctx );
	//	cl::Context *createContext( void (CL_CALLBACK * notifyFptr)( const char *, const void *, ::size_t, void *) = nullptr);
};

#endif // CLTOOL_HPP
