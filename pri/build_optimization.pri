
# This file contains build time optimizations
# NOTE: this isfor shorter build time, for optimized binary code generation see optimization.pri

PRECOMPILED_HEADER = $${SRCS}/libs/precompiled_headers.pch
CONFIG += precompile_header
