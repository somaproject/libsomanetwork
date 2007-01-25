CXXFLAGS = -I../../asio/asio-0.3.8rc1/include/ -g -O2 -D_REENTRANT
LDFLAGS = -lboost_thread -lboost_date_time -lboost_signals -lboost_program_options 
CXX = g++

OBJS = main.o network.o datareceiver.o
all: main

%.o : %.cc
	$(CXX) $(CXXFLAGS) -c $<

main: $(OBJS)
	$(CXX)  $(OBJS) $(LDFLAGS) -o main


datareceiver_test: datareceiver.o datareceiver_test.o
	$(CXX) datareceiver.o datareceiver_test.o $(LDFLAGS) -o datareceiver_test

network_test: datareceiver.o network.o network_test.o
	$(CXX) datareceiver.o network_test.o network.o $(LDFLAGS) -o network_test -lboost_unit_test_framework

network_bench: datareceiver.o network.o network_bench.o
	$(CXX) datareceiver.o network_bench.o network.o $(LDFLAGS) -o network_bench

tspipefifo_test: tspipefifo_test.o
	$(CXX) tspipefifo_test.o $(LDFLAGS) -o tspipefifo_test -lboost_unit_test_framework

tspipefifo_bench: tspipefifo_bench.o
	$(CXX) tspipefifo_bench.o $(LDFLAGS) -o tspipefifo_bench -lboost_unit_test_framework



deploy:
	cp main ~/acq/acq2
	cp main ~/acq/acq3
	cp main ~/acq/acq4
	cp main ~/acq/acq6
