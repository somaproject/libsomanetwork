#ifndef PACKETRECEIVER_H
#define PACKETRECEIVER_H

class PacketReceiver
{
 public:
  virtual void handleReceive() = 0; 

}; 

#endif // PACKETRECEIVER_H
