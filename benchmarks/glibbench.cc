#include <iostream>
#include <network.h>
#include <boost/program_options.hpp>

#include <unistd.h>     /* standard unix functions, like getpid()       */
#include <sys/types.h>  /* various type definitions, like pid_t         */
#include <signal.h>     /* signal name macros, and the kill() prototype */

#include <gtkmm/main.h>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace po = boost::program_options;

Network *  network; 
Glib::RefPtr<Glib::IOChannel> iochannel;
using namespace boost::posix_time; 

ptime t1(neg_infin); 
std::vector<int> rxCnt; 
int pktcnt = 0; 
int startchan, endchan;
Gtk::Main * app; 

bool DataRxCallback(Glib::IOCondition io_condition)
{

  if (t1.is_neg_infinity()) 
    {
      t1 = microsec_clock::local_time(); 
    }
  
  if ((io_condition & Glib::IO_IN) == 0) {
    std::cerr << "Invalid fifo response" << std::endl;
  }
  else 
    {
      char x; 
      read(network->getDataFifoPipe(), &x, 1); 
      
      DataPacket_t * rdp = network->getNewData(); 
      char chan = rdp->src; 
      pktcnt--; 
      rxCnt[chan - startchan]++; 
      if (pktcnt < 0) {
	app->quit(); 
      }
    }
  
  return true; 

}

void printDataStats()
{

  std::vector<DataReceiverStats> drs = network->getDataStats(); 
  std::vector<DataReceiverStats>::iterator i = drs.begin(); 
  for (i = drs.begin(); i != drs.end(); i++ ) 
    {
      std::cout << "src = " << i->source << "typ = " << i->type << std::endl;
      std::cout << "    " << "pktcount=" << i->pktCount 
		<< " latestSeq=" << i->latestSeq 
		<< " dupeCount=" << i->dupeCount
		<< " pendingCount=" << i->pendingCount 
		<< " mispktcnt = " << i->missingPacketCount 
		<< " reTxRxCount = " << i->reTxRxCount 
		<< " outOfOrderCount = " << i->outOfOrderCount 
		<< std::endl; 
    }
  

}

void catch_int(int sig_num)
{
  /* re-set the signal handler again to catch_int, for next time */
  signal(SIGINT, catch_int);
  /* and print the message */
  printDataStats(); 

  network->shutdown(); 
  
  exit(0); 
}


int main(int argc, char * argv[])
{
  
  app = new Gtk::Main(argc, argv); 

  //parse command options
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "produce help message")
    ("startchan", po::value<int>()->default_value(0), 
     "first channel to receive for")
    ("endchan", po::value<int>()->default_value(0), 
     "last channel to receive for")
    ("disableretx", po::value<int>()->default_value(0),
     "Request packet retransmission, 1=yes")
    ("packetcount", po::value<int>()->default_value(1000),
     "Wait for this many packets total"); 

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);    
  startchan = vm["startchan"].as<int>();
  endchan = vm["endchan"].as<int>(); 

  int N = endchan - startchan + 1; 
  pktcnt = vm["packetcount"].as<int>(); 

  std::cout << "Configuring rx for " << N << " receivers" << std::endl; 

  for (int i = 0; i < N; i++)
    {
      rxCnt.push_back(0); 
    }

  
  network =  new Network("127.0.0.1"); 
  
  for (int i = startchan; 
       i <= endchan; i++) {
    network->enableDataRX(i, charToDatatype(0)); // always assume type 0
  }
  
  network->run(); 
  
  // create timers
  
  
  // connect the signal handler
  int read_fd = network->getEventFifoPipe(); 
  
  Glib::signal_io().connect(sigc::ptr_fun(DataRxCallback), 
			    read_fd, Glib::IO_IN);
  
  // Creates a iochannel from the file descriptor
  iochannel = Glib::IOChannel::create_from_fd(read_fd);
  
  app->run(); 
  ptime t2(microsec_clock::local_time()); 
  network->shutdown(); 
  
  for (int i = vm["startchan"].as<int>(); 
       i <= vm["endchan"].as<int>(); i++) {
    int cnt = rxCnt[i - vm["startchan"].as<int>()]; 
    std::cout << "channel " << i  << " : " << 
      float(cnt) / (t2 -t1).total_microseconds()*1e6 
	      << " packets / second" << std::endl;
  }
  printDataStats();     
  
  return 0;
}
