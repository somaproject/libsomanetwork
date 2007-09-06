
PYNAME = `python -c "import sys; (a, b, c, t, n) = sys.version_info; print 'python%d.%d' % (a, b),"`


CFLAGS =  -fPIC -I/usr/include/$(PYNAME) -I/home/jonas/python/lib/python/numpy/core/include/ -pthread -O3 -g -pg `pkg-config --cflags somanetwork-1.0`

LDFLAGS = -pthread -shared -I/usr/include/$(PYNAME) -L/usr/lib -lpython2.5 `pkg-config --libs somanetwork-1.0` -g -pg  -lboost_python-gcc41-mt -lboost_thread-gcc41-mt

all: hello.so network.so

pyevent.o: pyevent.c
	gcc $(CFLAGS) -c pyevent.c


pyevent.so: pyevent.o
	gcc pyevent.o -o pyevent.so $(LDFLAGS)

