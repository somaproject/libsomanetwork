#!/usr/bin/python

import numpy as n
from struct import *

# first we generate a bunch of data with

WAVELEN = 32
tspikewave_desc = n.dtype([('valid', n.uint8),
                           ('null1', n.uint8),
                           ('null2', n.uint8),
                           ('null3', n.uint8),                           
                           ('filtid', '>i4'),
                           ('threshold', '>i4'),
                           ('wave', '%d>i4' % WAVELEN)])
tspike_desc = n.dtype([('typ', n.uint8),
                       ('src', n.uint8),
                       ('chanlen', n.uint16), 
                       ('time', '>u8'),
                       ('version', n.uint16),
                       ('x', tspikewave_desc),
                       ('y', tspikewave_desc),
                       ('a', tspikewave_desc),
                       ('b', tspikewave_desc) ])



if __name__ == "__main__":
    N = 10000

    x = n.zeros(N, dtype=tspike_desc)
    r = x.tostring()
    # make sure this is correct
    offset = 14
    assert ( (1 + 3 + 4 + 4 + WAVELEN*4 ) * 4 + offset == float(len(r))/N)
    print "PKTLEN = ", len(r)/N

    for  i in range(N):
        x[i]['typ'] = 0
        x[i]['src'] = (i % 256)
        x[i]['time'] = i * 10215 + 0x12345678
        for j, s in enumerate(['x', 'y', 'a', 'b']):
            x[i][s]['filtid'] = (j * i)
            x[i][s]['valid'] = (j * i  % 256)
            x[i][s]['threshold'] = (j * i* (2*17-141))
            for k in range(WAVELEN):
                x[i][s]['wave'][k] = (j * i * 0x12345 +  (k)) % 2**31
    print len(r)/N

    x.tofile('tspikes.frompy.dat')
