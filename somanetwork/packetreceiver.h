#ifndef SOMANETWORK_PACKETRECEIVER_H
#define SOMANETWORK_PACKETRECEIVER_H

namespace somanetwork {

class PacketReceiver
{
 public:
  virtual void handleReceive(int fd) = 0; 
  virtual ~PacketReceiver() {}; 
}; 

}

#endif // PACKETRECEIVER_H
