SRCDIR = ../src/
CXXFLAGS=-I$(SRCDIR) -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -D_POSIX_SOURCE -UNDEBUG -I../ -g

LDFLAGS=-lboost_unit_test_framework  -lhdf5_cpp -lhdf5  -lboost_filesystem -lhdf5_hl

all: runner event_test.o

tspike_test.o: tspike_test.cc
event_test.o: event_test.cc event.h

events.1.dat: event_test.py
	python event_test.py

frompy.dat: tspike_test.py
	python tspike_test.py

runner: runner.o  event_test.o tspike_test.o 


clean:
	rm *.o

