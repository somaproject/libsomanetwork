CXXFLAGS = -g -O0 -m64 -I../ -Wall
LDFLAGS = -lboost_thread -lboost_date_time  -lboost_program_options -lboost_unit_test_framework -g 

CXX = g++

all: runner

%.o : %.cc
	$(CXX) $(CXXFLAGS) -c $<

all: runner

tests.o: ../tests.cc
	$(CXX) $(CXXFLAGS) -c ../tests.cc

datareceiver.o: ../datareceiver.cc
	$(CXX) $(CXXFLAGS) -c ../datareceiver.cc

datareceiver_test.o: datareceiver_test.cc

eventreceiver.o: ../eventreceiver.cc
	$(CXX) $(CXXFLAGS) -c ../eventreceiver.cc

eventsender.o: ../eventsender.cc
	$(CXX) $(CXXFLAGS) -c ../eventsender.cc

eventdispatcher.o: ../eventdispatcher.cc
	$(CXX) $(CXXFLAGS) -c ../eventdispatcher.cc

eventreceiver_test.o: eventreceiver_test.cc 

eventtests: eventtests.o

runner.o: runner.cc

DATARXTEST = datareceiver.o datareceiver_test.o tests.o
EVENTRXTEST = eventreceiver.o eventreceiver_test.o eventtests.o 
EVENTTXTEST = eventsender.o eventsender_test.o
DISPATCH = eventdispatcher.o dispatcher_test.o

runner: runner.o $(DISPATCH) 

clean:
	rm *.o
