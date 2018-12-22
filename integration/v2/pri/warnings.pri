gcc{

#     ______
#    / ____/__    __
#   / /  __/ /___/ /_
#  / /__/_  __/_  __/
#  \____//_/   /_/
#


	# The most comprehensive and strict set of checks available
	QMAKE_CXXFLAGS += -pedantic -Wall -Wextra
	QMAKE_CXXFLAGS += -Wredundant-decls
	#QMAKE_CXXFLAGS += -Wsign-promo
	QMAKE_CXXFLAGS += -Wmissing-include-dirs   -Woverloaded-virtual
	QMAKE_CXXFLAGS += -Wformat=2 -Winit-self  -Wmissing-declarations
	QMAKE_CXXFLAGS += -Wcast-qual  -Wdisabled-optimization
	#QMAKE_CXXFLAGS += -Wcast-align # Warn about alignment changes due to casts. Not really useful at this time so commented out
	QMAKE_CXXFLAGS += -Wstrict-overflow=5
	#QMAKE_CXXFLAGS += -Wundef # Warn about referenced but undefined preprocessor defines

	# Mark no return in function returning non-void an ERROR
	QMAKE_CXXFLAGS += -Werror=return-type



	#Only relevant in GPP
	g++ {
			 QMAKE_CXXFLAGS += -Wstrict-null-sentinel
			 QMAKE_CXXFLAGS += -Wnoexcept
			 QMAKE_CXXFLAGS += -Wlogical-op
			 QMAKE_CXXFLAGS += -Wstack-usage=2048
	}

	# Some hotshot with a book suddenly got his very own warning in GCC attributed to it.
	#QMAKE_CXXFLAGS += -Weffc++

	# Remove redundant/annoying warnings
	QMAKE_CXXFLAGS += -Wno-padded
	QMAKE_CXXFLAGS -= -Wold-style-cast -Wctor-dtor-privacy -Wshadow -Wswitch-default
	QMAKE_CXXFLAGS -= -Wsign-conversion -Wno-gnu-array-member-paren-init
	QMAKE_CXXFLAGS += -Wno-unused-parameter -Wno-format-contains-nul  -Wno-format-security -Wno-vla
	# Brave...
	#QMAKE_CXXFLAGS += -Werror


	########## Here are some warnings disabled to reduce logging:


	# Stop whining about missing include dirs. It IS useful, but in our current system we add too many and there is no easy way to mend that.
	QMAKE_CXXFLAGS += -Wno-missing-include-dirs


#     ______
#    / ____/
#   / /
#  / /___
#  \____/
#


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
