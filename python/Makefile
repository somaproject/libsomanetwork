
PYNAME = `python -c "import sys; (a, b, c, t, n) = sys.version_info; print 'python%d.%d' % (a, b),"`


CCFLAGS =  -fPIC -I/usr/include/$(PYNAME) -I/home/jonas/python/lib/python/numpy/core/include/ -pthread -O3 -g -pg `pkg-config --cflags somanetwork-1.0`

LDFLAGS = -pthread -shared -I/usr/include/$(PYNAME) -L/usr/lib -lpython2.5 `pkg-config --libs somanetwork-1.0` -g -pg  -lboost_python -lboost_thread

all: hello.so network.so

hello.o: hello.cc
	g++ -fPIC -I/usr/include/$(PYNAME) -I/home/jonas/python/lib/python/numpy/core/include/ -pthread  -c hello.cc

hello.so: hello.o
	g++ hello.o -o hello.so -pthread -shared -I/usr/include/$(PYNAME) -L/usr/lib -lpython2.5 -lboost_python


network.o: network.cc
	g++ $(CCFLAGS) -c network.cc


network.so: network.o
	g++ network.o -o network.so $(LDFLAGS)

