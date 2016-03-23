all:
	g++ -O2 -ansi -pedantic -ldl -W -Wall -Wshadow -m64 -Wno-shadow      `pythia8-config --cxxflags`      `pythia8-config --libs --hepmc|sed 's|\-lhepmcinterface||'`       -L`pythia8-config --libdir`/archive -o example_main main_pythia_variousMatchMerge.cc
