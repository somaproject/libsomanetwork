#!/usr/bin/python

import numpy as n
from struct import *

# first we generate a bunch of data with

WAVELEN = 32
tspikewave_desc = n.dtype([('filtid', n.uint8),
                           ('valid', n.uint8),
                           ('threshold', '>i4'),
                           ('wave', '%d>i4' % WAVELEN)])
tspike_desc = n.dtype([('typ', n.uint8),
                       ('src', n.uint8),
                       ('chanlen', n.uint16), 
                       ('time', '>u8'),
                       ('x', tspikewave_desc),
                       ('y', tspikewave_desc),
                       ('a', tspikewave_desc),
                       ('b', tspikewave_desc) ])



if __name__ == "__main__":
    N = 100000

    x = n.zeros(N, dtype=tspike_desc)
    r = x.tostring()
    # make sure this is correct
    assert ( (1 + 1 + 4 + WAVELEN*4 ) * 4 + 1 +1 + 2 + 8 == len(r)/N)


    for  i in range(N):
        x[i]['typ'] = i % 37
        x[i]['src'] = (i % 256)
        x[i]['time'] = i * 10215
        for j, s in enumerate(['x', 'y', 'a', 'b']):
            x[i][s]['filtid'] = (j * i  % 256)
            x[i][s]['valid'] = (j * i  % 256)
            x[i][s]['threshold'] = (j * i* (2*17-141))
            for k in range(WAVELEN):
                x[i][s]['wave'][k] = j * i * 0x12345 +  (k)
    print len(r)/N

    x.tofile('frompy.dat')
