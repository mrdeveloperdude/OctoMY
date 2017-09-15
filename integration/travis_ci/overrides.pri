# This is the local_overrides.pri fil for use when building on Travis CI

# Build all tests that are meant to work in this kind of environment
# TODO: Look at enabling more tests like stress/3D/espeak etc.
DEFINES += USE_TESTS_BASIC

# Reduce build output to only warnings/errors to avoid the dreaded "log size exceeded 4 mb" error
CONFIG += silent


