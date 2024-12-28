// Uniform build options used by OctoMY
Properties {
	//cpp.commonCompilerFlags: cpp.commonCompilerFlags.concat(["-fno-exceptions", "-fno-rtti"]);
	//cpp.cxxFlags: cpp.cxxFlags.concat(["-fno-exceptions", "-fno-rtti"])
	//cpp.enableExceptions: false;
	//cpp.enableRtti: false;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Standard

/*
#Align these with the actual build of Qt in use (see image/scripts/qt.sh for options)

CONFIG -= rtti exceptions c++0x c++11 c++14 c++1y c++1z
CONFIG += qt # xcb debug thread 3dnow mmx stl sse sse2 largefile
CONFIG += c++14



# Support all kinds of architectures (universal builds etc)
CONFIG += x86 x86_64
#CONFIG += console

# Stop build if no std is selected
!contains(CONFIG, c++17){
	!contains(CONFIG, c++14){
		!contains(CONFIG, c++11){
			error( "CONFIG MUST CONTAIN c++11, c++14 or c++17. All are missing, aborting build!")
		}
	}
}


#     ______
#    / ____/__    __
#   / /  __/ /___/ /_
#  / /__/_  __/_  __/
#  \____//_/   /_/
#

# Strive towards the most modern standard of C++ language available that is compatible with our target
# From https://gcc.gnu.org/onlinedocs/gcc-5.1.0/gcc/C-Dialect-Options.html#C-Dialect-Options

QMAKE_CXXFLAGS -= -std=*
QMAKE_CXXFLAGS -= -std=c++98
QMAKE_CXXFLAGS -= -std=c++03
QMAKE_CXXFLAGS -= -std=gnu++98
QMAKE_CXXFLAGS -= -std=gnu++03
QMAKE_CXXFLAGS -= -std=c++11
QMAKE_CXXFLAGS -= -std=c++0x
QMAKE_CXXFLAGS -= -std=gnu++11
QMAKE_CXXFLAGS -= -std=gnu++0x
QMAKE_CXXFLAGS -= -std=c++14
QMAKE_CXXFLAGS -= -std=c++1y
QMAKE_CXXFLAGS -= -std=gnu++14
QMAKE_CXXFLAGS -= -std=gnu++1y
QMAKE_CXXFLAGS -= -std=c++1z
QMAKE_CXXFLAGS -= -std=gnu++1z
QMAKE_CXXFLAGS -= -std=c++17
QMAKE_CXXFLAGS -= -std=gnu++17

QMAKE_CXXFLAGS -= /std:c++14
QMAKE_CXXFLAGS -= /std:c++17
QMAKE_CXXFLAGS -= /std:c++latest

contains(CONFIG, c++17){
	gcc:QMAKE_CXXFLAGS += -std=c++17
	msvc:QMAKE_CXXFLAGS += /std:c++17
}
contains(CONFIG, c++14){
	gcc:QMAKE_CXXFLAGS += -std=c++14 -std=c++1y
	msvc:QMAKE_CXXFLAGS += /std:c++14
}
contains(CONFIG, c++11){
	gcc:QMAKE_CXXFLAGS += -std=c++11
}


#     ______
#    / ____/
#   / /
#  / /___
#  \____/
#

# Strive towards the most modern standard of pure C language available that is compatible with our target

QMAKE_CFLAGS -= -std=*
QMAKE_CFLAGS -= -std=c90
QMAKE_CFLAGS -= -std=c89
QMAKE_CFLAGS -= -std=iso9899:1990
QMAKE_CFLAGS -= -std=iso9899:199409
QMAKE_CFLAGS -= -std=c99
QMAKE_CFLAGS -= -std=c9x
QMAKE_CFLAGS -= -std=iso9899:1999
QMAKE_CFLAGS -= -std=iso9899:199x
QMAKE_CFLAGS -= -std=c11
QMAKE_CFLAGS -= -std=c1x
QMAKE_CFLAGS -= -std=iso9899:2011
QMAKE_CFLAGS -= -std=gnu90
QMAKE_CFLAGS -= -std=gnu89
QMAKE_CFLAGS -= -std=gnu99
QMAKE_CFLAGS -= -std=gnu9x
QMAKE_CFLAGS -= -std=gnu11
QMAKE_CFLAGS -= -std=gnu1x


gcc:QMAKE_CFLAGS += -std=c11
*/



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Optimization
/*
#     ______
#    / ____/__    __
#   / /  __/ /___/ /_
#  / /__/_  __/_  __/
#  \____//_/   /_/
#

# Here are C++ spesific optimization flags for different compilers

REMOVE_FLAGS += -O
REMOVE_FLAGS += -O2
REMOVE_FLAGS += -O3
REMOVE_FLAGS += -g

REMOVE_FLAGS += /Zi
REMOVE_FLAGS += /O1
REMOVE_FLAGS += /O2
REMOVE_FLAGS += /Ob
REMOVE_FLAGS += /Od
REMOVE_FLAGS += /Og
REMOVE_FLAGS += /Oi
REMOVE_FLAGS += /MT
REMOVE_FLAGS += /MD

REMOVE_FLAGS += -Zi
REMOVE_FLAGS += -O1
REMOVE_FLAGS += -O2
REMOVE_FLAGS += -Ob
REMOVE_FLAGS += -Od
REMOVE_FLAGS += -Og
REMOVE_FLAGS += -Oi
REMOVE_FLAGS += -MT
REMOVE_FLAGS += -MD

QMAKE_CXXFLAGS_DEBUG -= $$REMOVE_FLAGS
QMAKE_CXXFLAGS_RELEASE -= $$REMOVE_FLAGS
QMAKE_CXXFLAGS -= $$REMOVE_FLAGS
QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO -= $$REMOVE_FLAGS

QMAKE_CFLAGS_DEBUG -= $$REMOVE_FLAGS
QMAKE_CFLAGS_RELEASE -= $$REMOVE_FLAGS
QMAKE_CFLAGS -= $$REMOVE_FLAGS
QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO -= $$REMOVE_FLAGS

QMAKE_LFLAGS  -= $$REMOVE_FLAGS

#QMAKE_CXXFLAGS_RELEASE -= -O
#QMAKE_CXXFLAGS_RELEASE -= -O2
#QMAKE_CXXFLAGS_RELEASE -= -O3
#QMAKE_CXXFLAGS_RELEASE -= -g

#QMAKE_CXXFLAGS_RELEASE -= /Zi
#QMAKE_CXXFLAGS_RELEASE -= /O1
#QMAKE_CXXFLAGS_RELEASE -= /O2
#QMAKE_CXXFLAGS_RELEASE -= /Ob
#QMAKE_CXXFLAGS_RELEASE -= /Od
#QMAKE_CXXFLAGS_RELEASE -= /Og
#QMAKE_CXXFLAGS_RELEASE -= /Oi
#QMAKE_CXXFLAGS_RELEASE -= /MT
#QMAKE_CXXFLAGS_RELEASE -= /MD

#QMAKE_CXXFLAGS_DEBUG -= -O
#QMAKE_CXXFLAGS_DEBUG -= -O2
#QMAKE_CXXFLAGS_DEBUG -= -O3
#QMAKE_CXXFLAGS_DEBUG -= -g

#QMAKE_CXXFLAGS_DEBUG -= /Zi
#QMAKE_CXXFLAGS_DEBUG -= /O1
#QMAKE_CXXFLAGS_DEBUG -= /O2
#QMAKE_CXXFLAGS_DEBUG -= /Ob
#QMAKE_CXXFLAGS_DEBUG -= /Od
#QMAKE_CXXFLAGS_DEBUG -= /Og
#QMAKE_CXXFLAGS_DEBUG -= /Oi
#QMAKE_CXXFLAGS_DEBUG -= /MT
#QMAKE_CXXFLAGS_DEBUG -= /MD

#Make stacktrace beautiful dureing debugging
gcc:QMAKE_CXXFLAGS_COMMON += -fno-omit-frame-pointer

gcc:QMAKE_CXXFLAGS_RELEASE += -Os
gcc:QMAKE_CXXFLAGS_DEBUG += -g
msvc:QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO += -Os -g

msvc:QMAKE_CXXFLAGS_COMMON +=
msvc:QMAKE_CXXFLAGS_RELEASE += /O1 /MT
msvc:QMAKE_CXXFLAGS_DEBUG += /Zi /MTd
msvc:QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO += /O2 /MT /Zi

QMAKE_CXXFLAGS_RELEASE += $$QMAKE_CXXFLAGS_COMMON
QMAKE_CXXFLAGS_DEBUG+=    $$QMAKE_CXXFLAGS_COMMON



QMAKE_LFLAGS += $$QMAKE_CXXFLAGS


#     ______
#    / ____/
#   / /
#  / /___
#  \____/
#



# TODO: Add optimization flags for pure C

*/

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Warnings


/*
  
# ASCII FONT GENERATE WITH http://patorjk.com/software/taag/#p=display&f=Slant&t=C%2B%2B%20GCC


# We try to use the most comprehensive and strict set of compiler checks available and feasible

#     ______              __________________
#    / ____/__    __     / ____/ ____/ ____/
#   / /  __/ /___/ /_   / / __/ /   / /
#  / /__/_  __/_  __/  / /_/ / /___/ /___
#  \____//_/   /_/     \____/\____/\____/


# Here are C++ spesific warning flags for GCC

gcc{

	QMAKE_CXXFLAGS += -pedantic -Wall -Wextra
	QMAKE_CXXFLAGS += -Wredundant-decls
	#QMAKE_CXXFLAGS += -Wsign-promo
	QMAKE_CXXFLAGS += -Wmissing-include-dirs   -Woverloaded-virtual
	QMAKE_CXXFLAGS += -Wformat=2 -Winit-self  -Wmissing-declarations
	QMAKE_CXXFLAGS += -Wcast-qual  -Wdisabled-optimization
	#QMAKE_CXXFLAGS += -Wcast-align # Warn about alignment changes due to casts. Not really useful at this time so commented out
	
	#QMAKE_CXXFLAGS += -Wstrict-overflow=5 # Warn about tricky overflows. Not really useful at this time so commented out
	QMAKE_CXXFLAGS += -Wno-strict-overflow
	
	#QMAKE_CXXFLAGS += -Wundef # Warn about referenced but undefined preprocessor defines
	# Mark no return in function returning non-void an ERROR
	QMAKE_CXXFLAGS += -Werror=return-type

	# Only relevant in GPP
	g++ {
			 QMAKE_CXXFLAGS += -Wstrict-null-sentinel
			 QMAKE_CXXFLAGS += -Wnoexcept
			 QMAKE_CXXFLAGS += -Wlogical-op
			 QMAKE_CXXFLAGS += -Wstack-usage=2048
	}

	# Some hotshot with a book suddenly got his very own warning in GCC attributed to it.
	#QMAKE_CXXFLAGS += -Weffc++

	# Remove redundant/annoying warnings

	# QMAKE_CXXFLAGS += -Wno-covered-switch-defaults
	QMAKE_CXXFLAGS += -Wno-padded
	QMAKE_CXXFLAGS -= -Wold-style-cast -Wctor-dtor-privacy -Wshadow -Wswitch-default
	QMAKE_CXXFLAGS -= -Wsign-conversion -Wno-gnu-array-member-paren-init
	QMAKE_CXXFLAGS += -Wno-unused-parameter -Wno-format-contains-nul  -Wno-format-security -Wno-vla

	# Mark c++17 extensions to be welcome (no warnings or errors)
	# QMAKE_CXXFLAGS += -Wno-c++17-extensions

	# Brave...
	# QMAKE_CXXFLAGS += -Werror


	########## Here are some warnings disabled to reduce logging:


	# Stop whining about missing include dirs. It IS useful, but in our current system we add too many and there is no easy way to mend that.
	QMAKE_CXXFLAGS += -Wno-missing-include-dirs

}

#     ______   __________________
#    / ____/  / ____/ ____/ ____/
#   / /      / / __/ /   / /
#  / /___   / /_/ / /___/ /___
#  \____/   \____/\____/\____/
#

gcc{
	# NOTE: Only for pure C, since that are external dependencies only (which we don't want to change and so the warnings become nuicanses)
	QMAKE_CFLAGS += -Wno-parentheses
	QMAKE_CFLAGS += -Wno-int-conversion
	QMAKE_CFLAGS += -Wno-unused-value
	QMAKE_CFLAGS += -Wno-sign-compare

	#    Enable AddressSanitizer, a fast memory error detector. Memory access instructions will be instrumented to detect out-of-bounds and use-after-free bugs. See http://code.google.com/p/address-sanitizer/ for more details. The run-time behavior can be influenced using the ASAN_OPTIONS environment variable; see https://code.google.com/p/address-sanitizer/wiki/Flags#Run-time_flags for a list of supported options.
	#QMAKE_CXXFLAGS +=-fsanitize=kernel-address
	#    Enable AddressSanitizer for Linux kernel. See http://code.google.com/p/address-sanitizer/wiki/AddressSanitizerForKernel for more details.
	#QMAKE_CXXFLAGS +=-fsanitize=thread

	# THIS IS A WHOLE BUNCH OF DEBUG FEATURES IN GCC. UNCOMMENT TO ENTER THE SECRET WORLD OF GCC DEBUGGING
	#include(asan.pri)

	#WE MAY ACTUALLY HAVE SOME PURE C CODE

	QMAKE_CFLAGS += -Wno-unused-parameter -Wno-sometimes-uninitialized

	# Remove GCC-only stuff if other compilers are used. PS: contains() is used to avoid problems when ccache is used
	#!contains(QMAKE_CXX, g++){
	#	QMAKE_CXXFLAGS -= -fstack-usage -Wno-format-contains-nul
	#}

}

*/


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Linkage

/*
  
contains(DEFINES, USE_STATIC_LINKAGE){
	CONFIG += static
}

*/

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Protection

/*

#     ______
#    / ____/__    __
#   / /  __/ /___/ /_
#  / /__/_  __/_  __/
#  \____//_/   /_/
#

# Here are C++ spesific protection flags for different compilers


# Build in all available bug protection
#QMAKE_CXXFLAGS += -fstack-protector-all -fstrict-overflow -fstack-usage
#QMAKE_CXXFLAGS +=-fsanitize=address

USE_SANITIZER_MEMORY{

#    Enable AddressSanitizer, a fast memory error detector. Memory access instructions will be instrumented to detect out-of-bounds and use-after-free bugs. See http://code.google.com/p/address-sanitizer/ for more details. The run-time behavior can be influenced using the ASAN_OPTIONS environment variable; see https://code.google.com/p/address-sanitizer/wiki/Flags#Run-time_flags for a list of supported options.
#QMAKE_CXXFLAGS +=-fsanitize=kernel-address
#    Enable AddressSanitizer for Linux kernel. See http://code.google.com/p/address-sanitizer/wiki/AddressSanitizerForKernel for more details.
QMAKE_CXXFLAGS +=-fsanitize=thread
#    Enable ThreadSanitizer, a fast data race detector. Memory access instructions will be instrumented to detect data race bugs. See http://code.google.com/p/thread-sanitizer/ for more details. The run-time behavior can be influenced using the TSAN_OPTIONS environment variable; see https://code.google.com/p/thread-sanitizer/wiki/Flags for a list of supported options.
QMAKE_CXXFLAGS +=-fsanitize=leak
#    Enable LeakSanitizer, a memory leak detector. This option only matters for linking of executables and if neither -fsanitize=address nor -fsanitize=thread is used. In that case it will link the executable against a library that overrides malloc and other allocator functions. See https://code.google.com/p/address-sanitizer/wiki/LeakSanitizer for more details. The run-time behavior can be influenced using the LSAN_OPTIONS environment variable.
QMAKE_CXXFLAGS +=-fsanitize=undefined
#    Enable UndefinedBehaviorSanitizer, a fast undefined behavior detector. Various computations will be instrumented to detect undefined behavior at runtime. Current suboptions are:
QMAKE_CXXFLAGS +=    -fsanitize=shift
#        This option enables checking that the result of a shift operation is not undefined. Note that what exactly is considered undefined differs slightly between C and C++, as well as between ISO C90 and C99, etc.
QMAKE_CXXFLAGS +=    -fsanitize=integer-divide-by-zero
#       Detect integer division by zero as well as INT_MIN / -1 division.
QMAKE_CXXFLAGS +=    -fsanitize=unreachable
#        With this option, the compiler will turn the __builtin_unreachable call into a diagnostics message call instead. When reaching the __builtin_unreachable call, the behavior is undefined.
QMAKE_CXXFLAGS +=    -fsanitize=vla-bound
#        This option instructs the compiler to check that the size of a variable length array is positive. This option does not have any effect in -std=c++14 mode, as the standard requires the exception be thrown instead.
QMAKE_CXXFLAGS +=    -fsanitize=null
#        This option enables pointer checking. Particularly, the application built with this option turned on will issue an error message when it tries to dereference a NULL pointer, or if a reference (possibly an rvalue reference) is bound to a NULL pointer, or if a method is invoked on an object pointed by a NULL pointer.
QMAKE_CXXFLAGS +=    -fsanitize=return
#        This option enables return statement checking. Programs built with this option turned on will issue an error message when the end of a non-void function is reached without actually returning a value. This option works in C++ only.
QMAKE_CXXFLAGS +=    -fsanitize=signed-integer-overflow
#        This option enables signed integer overflow checking. We check that the result of +, *, and both unary and binary - does not overflow in the signed arithmetics. Note, integer promotion rules must be taken into account. That is, the following is not an overflow:
QMAKE_CXXFLAGS +=    -fsanitize=bounds
#        This option enables instrumentation of array bounds. Various out of bounds accesses are detected. Flexible array members and initializers of variables with static storage are not instrumented.
QMAKE_CXXFLAGS +=    -fsanitize=alignment
#        This option enables checking of alignment of pointers when they are dereferenced, or when a reference is bound to insufficiently aligned target, or when a method or constructor is invoked on insufficiently aligned object.
QMAKE_CXXFLAGS +=    -fsanitize=float-divide-by-zero
#        Detect floating-point division by zero. Unlike other similar options, -fsanitize=float-divide-by-zero is not enabled by -fsanitize=undefined, since floating-point division by zero can be a legitimate way of obtaining infinities and NaNs.
QMAKE_CXXFLAGS +=    -fsanitize=float-cast-overflow
#        This option enables floating-point type to integer conversion checking. We check that the result of the conversion does not overflow. This option does not work well with FE_INVALID exceptions enabled.
QMAKE_CXXFLAGS +=    -fsanitize=nonnull-attribute
#        This option enables instrumentation of calls, checking whether null values are not passed to arguments marked as requiring a non-null value by the nonnull function attribute.
QMAKE_CXXFLAGS +=    -fsanitize=returns-nonnull-attribute
#        This option enables instrumentation of return statements in functions marked with returns_nonnull function attribute, to detect returning of null values from such functions.

}




#     ______
#    / ____/
#   / /
#  / /___
#  \____/
#



# TODO: Add protection flags for pure C
  
  
*/

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Ccache

/*
gcc{

# Automatic support for ccache
CCACHE_EXISTS=$$system("which ccache >/dev/null 2>&1; echo $?")
equals(CCACHE_EXISTS, "0"){
	QMAKE_CXX="ccache $$QMAKE_CXX"
	#message("GOT CCACHE: $$QMAKE_CXX")
}

#message("QMAKE_CXX: $$QMAKE_CXX, CCACHE_EXISTS: $$CCACHE_EXISTS")
}

*/
