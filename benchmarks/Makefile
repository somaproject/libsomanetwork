CXXFLAGS = -g -pg -O2 -D_REENTRANT -m64 `pkg-config --cflags gtkmm-2.4 gdkmm-2.4 glibmm-2.4` -I../ -I../data/

LDFLAGS = -lboost_thread -lboost_date_time -lboost_program_options -pg `pkg-config --libs gtkmm-2.4 gdkmm-2.4 glibmm-2.4` 
CXX = g++

OBJS = glibbench.o

all: glibbench

%.o : %.cc
	$(CXX) $(CXXFLAGS) -c $<

network.o: ../network.cc
	$(CXX) $(CXXFLAGS) -c ../network.cc

datareceiver.o: ../datareceiver.cc
	$(CXX) $(CXXFLAGS) -c ../datareceiver.cc

eventreceiver.o: ../eventreceiver.cc
	$(CXX) $(CXXFLAGS) -c ../eventreceiver.cc

glibbench.o: glibbench.cc

glibbench: glibbench.o network.o datareceiver.o eventreceiver.o

clean:
	rm *.o
