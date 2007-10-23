#!/usr/bin/python
from threading import Thread
import network
import sys
import time
import cProfile
import gc

#gc.disable()

def main():
    pn = network.Network("testing")
    pn.setAllEventRX(True)

    print pn.run()

    els = []
    L = 100
    print "attempting ", L, " getEvents cycles"
    starttime = 0
    
    for i in range(L):
        els += pn.getEvents()
        
        print i,  time.time() - starttime 
        starttime = time.time()

main()
