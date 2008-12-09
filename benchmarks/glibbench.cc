#include <iostream>
#include <network.h>
#include <boost/program_options.hpp>

#include <unistd.h>   
#include <sys/types.h>  
#include <signal.h>   
#include <string>
#include "range.h" 
#include <gtkmm/main.h>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace po = boost::program_options;

Network *  network; 
Glib::RefPtr<Glib::IOChannel> iochannel;
using namespace boost::posix_time; 
using namespace std; 

ptime t1(neg_infin); 
typedef std::pair<datasource_t, datatype_t> dpair_t; 

std::map<dpair_t, int> rxCnt; 
std::map<dpair_t, int> rxCntMax; 
int totalpktcnt = 0; 

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
      
      pDataPacket_t rdp = network->getNewData(); 
      dpair_t pair = make_pair(rdp->src, rdp->typ);  
      rxCnt[pair]++; 
      if (rxCnt[pair] <= rxCntMax[pair]) {
	totalpktcnt--; 
      }

      if (totalpktcnt <= 0) {
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
    ("tspikes", po::value<std::string>(), 
     "range of tspike sources (inclusive)")
    ("tspikes-count", po::value<int>()->default_value(100), 
     "The # of TSpikes to receive per source")
    ("disableretx", po::value<int>()->default_value(0),
     "Request packet retransmission, 1=yes")
    ; 
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);    
  
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return 1;
  }
  

  std::vector<dpair_t> datasources; 


  std::vector<int> tchans = parserange(vm["tspikes"].as<string>()); 
  for (int i = 0; i < tchans.size(); i++) { 
    dpair_t dp = make_pair(i, charToDatatype(0)); 
    datasources.push_back(dp); 
    int num = vm["tspikes-count"].as<int>(); 
    
    totalpktcnt += num; 
    rxCntMax[dp] = num; 
    rxCnt[dp] = 0; 
  }

  std::cout << "Configuring rx for " << tchans.size() << " TSpike receivers" << std::endl; 
  
  
  network =  new Network("127.0.0.1"); 

  // enable tspikes

  for (int i = 0; i < datasources.size(); i++) {
    network->enableDataRX(datasources[i].first, datasources[i].second); 
  }
  
  network->run(); 
  
  // create timers
  
  
  // connect the signal handler
  int read_fd = network->getDataFifoPipe(); 
  
  Glib::signal_io().connect(sigc::ptr_fun(DataRxCallback), 
			    read_fd, Glib::IO_IN);
  
  // Creates a iochannel from the file descriptor
  iochannel = Glib::IOChannel::create_from_fd(read_fd);
  
  app->run(); 
  ptime t2(microsec_clock::local_time()); 
  network->shutdown(); 
  
  for (int i = 0; i < datasources.size(); i++) {
    int cnt = rxCnt[datasources[i]]; 

    std::cout << "datasource  src=" << (int) datasources[i].first 
	      << " typ=" << (int) datasources[i].second << " : " << 
      float(cnt) / (t2 -t1).total_microseconds()*1e6 
	      << " packets / second" << std::endl;
  }
  printDataStats();     
  
  return 0;
}
