# Simple make file showing the commandline arguments to g++
# used to compile a benchmark with gauge.
# Note, that the paths to the gauge and boost libraries depend
# on your specific machine and will need to be updated .

all:
	g++ main.cpp -o benchmark --std=c++0x -I../../src -L../../build/linux -lgauge -L../../build/linux/__root__/home/mvp/dev/bundle_dependencies/boost-1.2.0/ -lboost_chrono -lboost_program_options -lboost_system -lrt
