#!/usr/bin/python
from threading import Thread
import hello
import time

class threadtest(Thread):
    def __init__(self, instance):
        Thread.__init__(self)
        self.instance = instance
        
    def run(self):
        for i in range(10):
            print "instance =", self.instance, i
            time.sleep(1)

t1 = threadtest(1)
t2 = threadtest(2)
t1.start()
t2.start()
print hello.freeze2()
t1.join()
t2.join()
