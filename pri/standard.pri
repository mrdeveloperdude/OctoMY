#Align these with the actual build of Qt in use (see image/scripts/qt.sh for options)

CONFIG -= rtti exceptions c++0x c++11 c++14 c++1y c++1z
CONFIG += qt # xcb debug thread 3dnow mmx stl sse sse2 largefile
CONFIG += c++14


#
#CONFIG += static
# Support all kinds of architectures (universal builds etc)
CONFIG += x86 x86_64
#CONFIG += console

# Stop build if no std is selected
!contains(CONFIG, c++14){
	!contains(CONFIG, c++11){
		error( "CONFIG MUST CONTAIN c++11 or c++14. Both are missing, aborting build!")
	}
}





# Strive towards the most modern standard of C++ language available
# From https://gcc.gnu.org/onlinedocs/gcc-5.1.0/gcc/C-Dialect-Options.html#C-Dialect-Options
#     ______
#    / ____/__    __
#   / /  __/ /___/ /_
#  / /__/_  __/_  __/
#  \____//_/   /_/
#
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

contains(CONFIG, c++14){
	QMAKE_CXXFLAGS += -std=c++14 -std=c++1y
}
contains(CONFIG, c++11){
	QMAKE_CXXFLAGS += -std=c++11
}


# Strive towards the most modern standard of pure C language available
#     ______
#    / ____/
#   / /
#  / /___
#  \____/
#
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


QMAKE_CFLAGS += -std=c11


#PRECOMPILED_HEADER = $${SRCS}/libs/precompiled_headers.pch
#CONFIG += precompile_header
