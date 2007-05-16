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

datareceiver.o: ../datareceiver.cc
	$(CXX) $(CXXFLAGS) -c ../datareceiver.cc

datareceiver_test.o: datareceiver_test.cc

runner.o: runner.cc

runner: runner.o datareceiver.o datareceiver_test.o tests.o


clean:
	rm *.o
