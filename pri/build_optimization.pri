
# This file contains build time optimizations
# NOTE: this is for shorter build time, for optimized binary code generation see optimization.pri

#PRECOMPILED_HEADER = $${SRCS}/libs/precompiled_headers.pch
#CONFIG += precompile_header

# Make sure to mirror project filesystem in output dir.
# This will avoid errors when sources with the same name exist
CONFIG += object_parallel_to_source
