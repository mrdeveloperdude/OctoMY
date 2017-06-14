
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

