from somapynet import eaddr 
from somapynet.neteventio import NetEventIO     
import time

eio = NetEventIO("10.0.0.2")

#timerid = 0x4A
#timecmd = 0x30
timerid = 0x00
timecmd = 0x10

eio.addRXMask(timecmd, timerid)

eio.start()

i = 0
N = 10000
ts1 = time.time()

while i < N:
    erx = eio.getEvents()
    for event in erx:
        print event
        i+= 1
ts2 = time.time()

eio.stop()

print ts2 - ts1
