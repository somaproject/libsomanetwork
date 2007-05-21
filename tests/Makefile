CXXFLAGS = -g -pg -O2 -D_REENTRANT -m64 -I../
LDFLAGS = -lboost_thread -lboost_date_time  -lboost_program_options -pg -lboost_unit_test_framework -g

CXX = g++

OBJS = main.o network.o datareceiver.o tests.o
all: runner

%.o : %.cc
	$(CXX) $(CXXFLAGS) -c $<

all: runner

tests.o: ../tests.cc
	$(CXX) $(CXXFLAGS) -c ../tests.cc

eventtests.o: ../eventtests.cc
	$(CXX) $(CXXFLAGS) -c ../eventtests.cc

datareceiver.o: ../datareceiver.cc
	$(CXX) $(CXXFLAGS) -c ../datareceiver.cc

datareceiver_test.o: datareceiver_test.cc

eventreceiver.o: ../eventreceiver.cc
	$(CXX) $(CXXFLAGS) -c ../eventreceiver.cc

eventreceiver_test.o: eventreceiver_test.cc

runner.o: runner.cc

runner: runner.o tests.o eventreceiver.o eventreceiver_test.o


clean:
	rm *.o
