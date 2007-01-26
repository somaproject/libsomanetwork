#include "tests.h"
#include "datareceiver.h"

char * fakeDataPacket(unsigned int seq, char src, char typ)
{
  char * data = new char[BUFSIZE]; 
  unsigned int seqh = htonl(seq); 
  memcpy(data, &seqh,  4); 
  data[4] = typ; 
  data[5] = src; 

  return data; 

}
