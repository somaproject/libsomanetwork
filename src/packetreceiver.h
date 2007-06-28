#ifndef PACKETRECEIVER_H
#define PACKETRECEIVER_H

class PacketReceiver
{
 public:
  virtual void handleReceive(int fd) = 0; 
  virtual ~PacketReceiver() {}; 
}; 

#endif // PACKETRECEIVER_H
