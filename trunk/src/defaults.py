# Default config for Aegis
import os

# Build to build-arch-os dir
BUILDDIR=1

# Use optimizations
RELEASE=0

# Install prefix
PREFIX='/usr/local'

# Where to install binaries
BINDIR='${PREFIX}/bin'

# Where data files are install to
DATADIR='${PREFIX}/share/museek'

# Import local settings from previous build
try:
    exec(open("mulocal.py").read())
except IOError:
    pass
