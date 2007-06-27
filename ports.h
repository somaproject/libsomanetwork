#ifndef PORTS_H
#define PORTS_H

/*

The canonical reference for all soma ports. 

*/ 

// FROM SOFTWARE TO SOMA
// These are ports that we, the software, send TO on the Soma hardware. 
// That is, the soma hardware is listening on these ports

const DATARETXPORT = 4400; 

const EVENTRXRETXPORT = 5500; // event retransmission requests

const EVENTTXPORT = 5100; // events from software to the soma bus

// FROM SOMA TO SOFTWARE
// These are ports that we, the software, listen on. Soma sets these as the 
// DESTINATION port in packets that it sends. 


inline int dataPortLookup(int type, int source) {
  return 4000  + type*64 + source;  
}

const EVENTRXPORT = 5000; 



#endif // PORTS_H
