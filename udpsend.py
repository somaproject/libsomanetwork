#!/usr/bin/python
import socket
import struct
mySocket = socket.socket ( socket.AF_INET, socket.SOCK_DGRAM )

host = "localhost"
port = 4000
buf = 1024
addr = (host,port)

id = 12345678
src = 7
typ = 3

for i in range(100000):
    
    dataheader = struct.pack(">IBB", id + i, src, typ)
    datastring = "A"*600
    data = dataheader + datastring

    mySocket.sendto(data, addr)
