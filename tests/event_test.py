#!/usr/bin/python

import numpy as n
from struct import *

# first we generate a bunch of data with
event_desc = n.dtype([('cmd', n.uint8),
                      ('src', n.uint8),
                      ('data', '5>i2')])



def genEventPacket(seq, eventsets):
    outbuf = ""
    outbuf += pack("!I", seq)
    for evtset in eventsets:
        if evtset == []:
            l = 0
        else:
            l = len(evtset) 
        outbuf += pack("!h", l)
        for e in evtset:
            outbuf += e.tostring()
    
    return outbuf
        
if __name__ == "__main__":

    # generate a series of packets

    esetlens = [5, 17, 41, 23,  3]
    esets = []
    for esetnum in esetlens:
        eset = []
        x = n.zeros(esetnum, dtype=event_desc)
        for i in range(esetnum):
            x[i]['cmd'] = i
            x[i]['src'] = (i * 7) % 256
            for k in range(5):
                x[i]['data'][k] = i +  k * 718 + 314

        esets.append(x)
        
    s = genEventPacket(0x12345678, esets)

    fid = file("events.1.dat", 'w')
    fid.write(s)
    fid.close()

    
    
