CXXFLAGS = -g -pg -O2 -D_REENTRANT -m64
LDFLAGS = -lboost_thread -lboost_date_time -lboost_signals -lboost_program_options -g -levent
CXX = ccache g++

#OBJS = 
all: libeventtest

libeventtest: libeventtest.o 



# datareceiver_test: datareceiver.o datareceiver_test.o tests.o
# 	$(CXX) datareceiver.o datareceiver_test.o tests.o $(LDFLAGS) -o datareceiver_test -lboost_unit_test_framework

# network_test: datareceiver.o network.o network_test.o tests.o
# 	$(CXX) datareceiver.o network_test.o network.o tests.o $(LDFLAGS) -o network_test -lboost_unit_test_framework

# network_bench: datareceiver.o network.o network_bench.o
# 	$(CXX) datareceiver.o network_bench.o network.o $(LDFLAGS) -o network_bench

# tspipefifo_test: tspipefifo_test.o
# 	$(CXX) tspipefifo_test.o $(LDFLAGS) -o tspipefifo_test -lboost_unit_test_framework

# tspipefifo_bench: tspipefifo_bench.o
# 	$(CXX) tspipefifo_bench.o $(LDFLAGS) -o tspipefifo_bench -lboost_unit_test_framework

# tests: datareceiver_test.o network_test.o datareceiver.o network.o tests.o
# 	$(CXX) datareceiver_test.o network_test.o network.o datareceiver.o tests.o $(LDFLAGS) -o tests -lboost_unit_test_framework

# deploy: network_bench
# 	cp network_bench ~/acq/acq3
# 	cp network_bench ~/acq/acq4
# 	cp network_bench ~/acq/acq6

clean:
	rm *.o
