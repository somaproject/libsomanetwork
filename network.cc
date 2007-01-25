#include <boost/thread/thread.hpp>
#include "network.h"
#include "datareceiver.h"


Network::Network()
{
}

void Network::run()
{
  pthrd_ = new boost::thread(boost::bind(&Network::workthread,
				 this));
  
}
void Network::workthread()
{
  ioservice_.run(); 
}

void Network::shutdown()
{
  ioservice_.stop(); 

}

Network::~Network()
{
  pthrd_->join(); 
}


void Network::appendDataOut(RawData* out) {

  outputFifo_.append(out); 

}

RawData* Network::getNewData(void)
{
  return outputFifo_.pop(); 
}

int Network::getTSPipeFifoPipe()
{
  return outputFifo_.readingPipe; 
}

void Network::enableDataRx(datasource_t src, datatype_t typ)
{
  datagen_t dg(src, typ); 

  dataReceivers_[dg] = new DataReceiver(ioservice_, src, typ, 
					 boost::bind(&Network::appendDataOut, 
						     this, _1) ); 
  
}

void Network::disableDataRx(datasource_t src, datatype_t typ)
{
  // we really need a custom try/catch here 
  datagen_t dg(src, typ); 
  DataReceiver* dr = dataReceivers_[dg]; 
  std::map<const datagen_t, DataReceiver*>::iterator i = dataReceivers_.find(dg); 
  delete dr;   
}
