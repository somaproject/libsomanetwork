#include <iostream>
#include <boost/program_options.hpp>
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/fstream.hpp"  
#include "boost/algorithm/string/split.hpp" 
#include <boost/algorithm/string.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <somanetwork/datapacket.h>
#include <somanetwork/tspike.h>
#include <somanetwork/wave.h>
#include <somanetwork/raw.h>
#include <somanetwork/ports.h>
#include <limits>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <time.h>
#include "range.h"
#include "eventsynthtx.h"
#include "audiogen.h"
#include "digitaloutgen.h"

using namespace boost::filesystem; 
using namespace std; 
namespace po = boost::program_options;

struct txparams_t
{
  datasource_t src; 
  datatype_t typ; 
  double period; 
  double elapsed; 
  sequence_t seq; 
  sockaddr_in si_addr; 
  somatime_t somatime; 
  int count; 
  int socket; 
  char * pdata; // FIXME stupid hack of an opaque pointer to the data struct

}; 


void data_send_packet(txparams_t *  params) {
  /*
    Handle all packet transmission, sequence updates, etc. 

  */
  
  pDataPacket_t p;
  size_t len; 
  char buffer[BUFSIZE+4]; 
  
  if (params->typ == TSPIKE) {
    TSpike_t * ts = (TSpike_t*)params->pdata; 
    ts->src = params->src; 
    ts->time = 0; 

    ts->x.valid = 1; 
    ts->x.filtid = 0x12345678; 
    ts->x.threshold = 100000; 
    
    ts->y.valid = 1; 
    ts->y.filtid = 0xAABBCCDD;
    ts->y.threshold = 100000; 
    
    ts->a.valid = 1; 
    ts->a.filtid = 0x11223344;
    ts->a.threshold = 100000; 
    
    ts->b.valid = 1; 
    ts->b.filtid = 0xABCD1234; 
    ts->b.threshold = 100000; 
    
    for (int i = 0; i < TSPIKEWAVE_LEN; i++) {
      ts->x.wave[i] = i * 1000; 
      ts->y.wave[i] = i * 2000; 
      ts->a.wave[i] = i * 3000; 
      ts->b.wave[i] = i * 4000; 

    }

    p = rawFromTSpikeForTX(*ts, params->seq, &len); 
    // add the sequence number
    sequence_t seq = p->seq; 
    int nseq = htonl(seq); 
    memcpy(buffer, &nseq, sizeof(nseq)); 
    // put in the rest of the data
    memcpy(&buffer[4], &(p->body[0]), len); 
    
    // now the actual TX 
    
    ssize_t result = sendto(params->socket, buffer, len+4, 0, 
			    (sockaddr*) &(params->si_addr), sizeof(params->si_addr)); 
    params->seq++; 
    
  } else if (params->typ == WAVE) {

    Wave_t * wave = (Wave_t*)params->pdata; 
    wave->src = params->src; 
    wave->time = 0; 

    wave->sampratenum =2000; 
    wave->samprateden = 1; 
    wave->selchan = 0; 
    for (int i = 0; i < WAVEBUF_LEN; i++) {
      wave->wave[i] = params->seq + i * 1000; 
    }
    
    //p = rawFromWaveForTX(*wave, params->seq, &len); 

  } else if (params->typ == RAW) {
    // raw is a bit more complex, as we get to create
    // several 

    // handling time here is tricky

    for(int i = 0; i < 4; i++) {
      pDataPacket_t p;
      Raw_t *raws = (Raw_t*)(params->pdata); 
      Raw_t * raw = &(raws[i]); 

      raw->chansrc = i; 
      raw->time = params->somatime;
      double period = 32*(i + 1);
      
      for (int i = 0; i < RAWBUF_LEN; i++) {
	raw->data[i] =  int32_t(10000000 * (sin(2 * 3.141592 * double(i) / period))); 
      }

      p = rawFromRawForTX(*raw, params->seq, &len); 
      // add the sequence number
      sequence_t seq = p->seq; 
      int nseq = htonl(seq); 
      memcpy(buffer, &nseq, sizeof(nseq)); 
      // put in the rest of the data
      memcpy(&buffer[4], &(p->body[0]), len); 
      
      // now the actual TX 
      ssize_t result = sendto(params->socket, buffer, len+4, 0, 
			      (sockaddr*) &(params->si_addr), sizeof(params->si_addr)); 
      params->seq++; 
    }
    params->somatime += 200; 
    
  }
  

}

void data_setup_socket(txparams_t *  params, std::string destipaddr) {
  /*
    open the socket 
    set the flags
  */ 
  

  short port = dataPortLookup(params->typ, params->src); 
  

  /* Create the UDP socket */
  params->socket =  socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP); 
  int optval = 1; 
  int res; 
  optval = 1; 
  setsockopt(params->socket, SOL_SOCKET, SO_REUSEADDR, 
	     &optval, sizeof (optval)); 
  
  optval = 1; 
  res = setsockopt(params->socket, SOL_SOCKET, SO_BROADCAST, 
	     &optval, sizeof (optval)); 

  /* Construct the server sockaddr_in structure */
  memset(&(params->si_addr), 0, sizeof(params->si_addr));
  
  params->si_addr.sin_family = AF_INET; 
  params->si_addr.sin_addr.s_addr  = inet_addr(destipaddr.c_str());
  params->si_addr.sin_port = htons(port); 
  
  // Now set up the constant data packet. 
  if (params->typ == TSPIKE) {
    TSpike_t * pdata = new TSpike_t(); 
    pdata->src = params->src; 
    params->pdata = (char *) pdata; 

  } else if (params->typ == WAVE) {
    // FIXME
  } else if (params->typ == RAW) {
    Raw_t * pdata = new Raw_t[4]; 
    pdata->src = params->src; 
    params->pdata = (char*)pdata; 
  }
}

void datatx_thread(vector<txparams_t> &  txparams)
{
  /**
   *  txparams is the vector of transmit parameters for this thread
   *
   *
   */
  
  timeval time; 
  gettimeofday(&time, NULL); 
  sequence_t seq = 0; 
  
  // first, find the max
  vector<txparams_t>::iterator pos; 
  int count = 0; 
  for(pos = txparams.begin(); pos != txparams.end(); pos++) {
    if (pos->count > count) {
      count = pos->count; 
    }
    pos->elapsed = pos->period; 
    pos->seq = 0; 
    
  }
  
  while(1) {
    timeval curtime; 
    gettimeofday(&curtime, NULL); 

    double elapsedtimeus = usecdelta(time, curtime); 
    double elapsedtime = elapsedtimeus / 1e6; 
    for(pos = txparams.begin(); pos != txparams.end(); pos++) {
      if (pos->count > 0) {
	// data_send_packet(i)  // 
	pos->elapsed += elapsedtime; 
	while(pos->elapsed > 0) {

	  pos->elapsed -= pos->period; 
	  data_send_packet(&(*pos)); 	  
	  pos->count--; 
	}

      }
      usleep(1000); 
    }
    
    time = curtime; 
    bool alldone= true; 
    for(int i = 0; i < txparams.size(); i++) {
      if (txparams[i].count > 0) {
	alldone= false; 
      }
      
    }
    if (alldone)
      break; 

  }

}

int main(int argc, char * argv[])
  {
 
  // Declare the supported options.
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "produce help message")
    ("destip", po::value<string>()->default_value("127.0.0.1"), 
     "IP address to send data to")    
    ("threads", po::value<int>()->default_value(1), 
     "number of independent TX threads")
    ("tspikes", po::value<string>(), 
     "range of tspike sources (inclusive)")
    ("tspikes-rate", po::value<int>()->default_value(100), 
     "The rate of TSpike transmission in Hz")
    ("tspikes-count", po::value<int>()->default_value(100), 
     "The # of TSpikes to send per source")
    ("waves", po::value<string>(), 
     "range of wave sources (inclusive)")
    ("waves-rate", po::value<int>()->default_value(100), 
     "The rate of wave transmission in Hz")
    ("waves-count", po::value<int>()->default_value(100), 
     "The # of Waves to send per source")
    ("raws", po::value<string>(), 
     "range of raw sources (inclusive)")
    ("raws-count", po::value<int>()->default_value(100), 
     "The # of raw packets to send per source")
    ("send-events", "send timer events")
    ("enable-tracker-events", "send video tracker events from digital out")

    ("enable-audio-events", "send audio events")
    ("audio-freq", po::value<float>()->default_value(1000.0), 
     "audio at this freq")
    ;
  

  po::variables_map vm;

  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);    
  

  if (vm.count("help")) {
    cout << desc << "\n";
    return 1;
  }

  std::vector<txparams_t>  txparams; 
  
  if (vm.count("tspikes")) {
    vector<int> tgt = parserange(vm["tspikes"].as<string>()); 

    double period = 1.0 / vm["tspikes-rate"].as<int>(); 
    for (vector<int>::iterator i = tgt.begin(); i != tgt.end(); i++) {
      txparams_t tx; 
      tx.src = *i; 
      tx.typ = TSPIKE; 
      tx.period = period; 
      tx.count = vm["tspikes-count"].as<int>(); 
      txparams.push_back(tx); 
     }
  }
  if (vm.count("raws")) {
    vector<int> tgt = parserange(vm["raws"].as<string>()); 

    for (vector<int>::iterator i = tgt.begin(); i != tgt.end(); i++) {
      txparams_t tx; 
      tx.src = *i; 
      tx.typ = RAW; 
      tx.period = 1.0 / 4000.0; 
      tx.count = vm["raws-count"].as<int>(); 
      tx.somatime = 0; 
      txparams.push_back(tx); 
     }
  }

  // FIXME: Waves

  
  for(vector<txparams_t>::iterator i = txparams.begin(); i != txparams.end(); i++)
    {
      data_setup_socket(&(*i), vm["destip"].as<string>()); 
    }
  
  // "thread-less" 
  int THREADN = vm["threads"].as<int>(); 

  boost::thread_group tgroup; 

  boost::thread * eventtxthread  = NULL; 

  if(vm.count("send-events")) {
    // always send events in separate thread
    eventtxparams_t * etxp = new eventtxparams_t; 
    etxp->elapsed = 0.0; 
    etxp->seq = 0; 
    etxp->ecycleload = 1; 
    etxp->count = 0; 
    etxp->latesttime = 0; 

    event_setup_socket(etxp, vm["destip"].as<string>()); 

    if(vm.count("enable-audio-events")) {
      float freq = vm["audio-freq"].as<float>(); 
      AudioGenerator * ag = new AudioGenerator(0, freq); 
      etxp->eventSources_.push_back(ag); 
    }

    if(vm.count("enable-tracker-events")) {
      DigitalOutputGenerator::patternList_t lst;
      std::list<int> initvals;  
      // video trigger 
      lst.push_back(PeriodicPattern(0, 250, 10, 0.2)); 
      initvals.push_back(0); 
      // front diode
      lst.push_back(PeriodicPattern(1, 500, 0, 0.4)); 
      initvals.push_back(0);  
      // back diode
      lst.push_back(PeriodicPattern(2, 500, 250, 0.4)); 
      initvals.push_back(1); 

      DigitalOutputGenerator * dog = new DigitalOutputGenerator(lst, initvals); 
      etxp->eventSources_.push_back(dog); 
    }

    eventtxthread = new boost::thread(boost::bind(eventtx_thread, 
						  etxp)); 
  }

  // now the data threads

  int pos = 0; 
  int txchunksize = txparams.size() / THREADN ; 

  for(int i = 0; i < THREADN; i++) {
    vector<txparams_t>  * thread_txparams = new vector<txparams_t>(); 
    thread_txparams->reserve(txchunksize); 

    // copy the data into the params
    for (int j = 0; j < txchunksize; j++) {
      if (pos +j < txparams.size()) {
	thread_txparams->push_back(txparams[pos +j]); 
      }
      
    }
    pos += txchunksize; 
    
    boost::thread * t = new boost::thread(boost::bind(datatx_thread, *thread_txparams));  
    tgroup.add_thread(t); 
    
  }
  

  tgroup.join_all();
  
  // done

}
