# Simple make file showing the commandline arguments to g++
# used to compile a simple example with kodo
# Note, that the paths to the libraries depend
# on your specific machine and will need to be updated.

# The include path to the kodo sources
kodo_dir = ../../src

# The include path to the sak sources
sak_dir = ~/dev/bundle_dependencies/sak-master/src

# The include path to the fifi sources
fifi_dir = ~/dev/bundle_dependencies/fifi-master/src

# The include path to the boost sources
boost_dir = ~/dev/bundle_dependencies/boost-master/boost-lib

# Invoke the compiler
all:
	g++ main.cpp -o test --std=c++0x -I $(boost_dir) -I $(fifi_dir) -I $(kodo_dir) -I $(sak_dir)
