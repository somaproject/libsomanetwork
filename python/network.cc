#include <boost/python.hpp>
#include <boost/thread/thread.hpp>
#include <string>
#include <iostream> 
#include <somanetwork/network.h>
#include <set>
#include <list>
#include <sys/select.h>


using namespace boost::python;

typedef std::set<eventcmd_t> cmdset_t; 

char const* greet()
{
   return "network, world";
}


struct PyEvent_t {
  eventcmd_t cmd; 
  eventsource_t src; 
  uint16_t data[EVENTLEN-1]; 
};
 

class PyNetwork {

public: 
  PyNetwork(std::string txt); 
  std::string name_; 
  void run(); 
  boost::python::list getEvents(); 
  ~PyNetwork(); 
  void enableEventRX(eventcmd_t cmd); 
  void disableEventRX(eventcmd_t cmd); 
  void setAllEventRX(bool state); 
  

private:
  Network network_; 
  bool rxall_; 
  cmdset_t cmdset_; 
  
  std::list<PyEvent_t> pyEventList_; 
  int pyEventListCnt_; 
  boost::mutex pyEventListMutex_; 
  boost::condition eventsAvailable_; 
  boost::mutex eventsAvailableMutex_; 
  boost::python::list eventListToPyList(); 
  void eventListToPyListP(  boost::python::list *); 
  void rxEventsThread(); 
  bool acceptEvent(Event_t event); 
  boost::thread * pRXEventsThread_; 

}; 

PyNetwork::PyNetwork(std::string txt)  :
  network_(txt), 
  rxall_ (false), 
  pyEventListCnt_(0)
{
  name_ = txt; 
}

PyNetwork::~PyNetwork() {
  network_.shutdown(); 
  EventReceiverStats ers = network_.getEventStats(); 
  std::cout << "Event stats:" << std::endl; 
  std::cout << "--------------------------------------------------------"
	    << std::endl; 


  std::cout << "pktCount = " << ers.pktCount << std::endl; 
  std::cout << "latestSeq = " << ers.latestSeq << std::endl; 
  std::cout << "dupeCount = " << ers.dupeCount << std::endl; 
  std::cout << "pendingCount = " << ers.pendingCount 
	    << std::endl; 
  std::cout << "missingPacketCount = " 
	    << ers.missingPacketCount << std::endl; 
  std::cout << "reTxRxCount = " << ers.reTxRxCount << std::endl; 
  std::cout << "outOfOrderCount = " << ers.outOfOrderCount << std::endl; 



}


boost::python::list PyNetwork::eventListToPyList()
{

  boost::python::list pylist; 

  boost::mutex::scoped_lock lock(pyEventListMutex_); 

  for(std::list<PyEvent_t>::iterator i = pyEventList_.begin(); 
      i != pyEventList_.end(); i++)
     {
       pylist.append(*i); 
     }
   pyEventList_.clear();

   pyEventListCnt_ = 0; 
  
  return pylist; 
}

void PyNetwork::eventListToPyListP(boost::python::list * pylist)
{

  boost::mutex::scoped_lock lock(pyEventListMutex_); 

  for(std::list<PyEvent_t>::iterator i = pyEventList_.begin(); 
      i != pyEventList_.end(); i++)
     {
       pylist->append(*i); 
     }
   pyEventList_.clear();

   pyEventListCnt_ = 0; 
  
}


boost::python::list PyNetwork::getEvents()
{

  boost::python::list l; 
  //// wait on condition variablle
  Py_BEGIN_ALLOW_THREADS; 
  boost::mutex::scoped_lock lock(eventsAvailableMutex_); 
  eventsAvailable_.wait(lock); 


  Py_END_ALLOW_THREADS; 
  
  
  eventListToPyListP(&l); 
  return l; 
}

void PyNetwork::run() {
  network_.run(); 
  // start a separate thread for event processing
  pRXEventsThread_ = new boost::thread(boost::bind(&PyNetwork::rxEventsThread, 
						   this));
  
}


bool PyNetwork::acceptEvent(Event_t event) {
  // Does event appear on our list of events to accept? 
  if (rxall_)
    return true; 
  
  return cmdset_.find(event.cmd)  != cmdset_.end(); 
  
}

void PyNetwork::enableEventRX(eventcmd_t cmd)
{
  cmdset_.insert(cmd); 
}
void PyNetwork::disableEventRX(eventcmd_t cmd)
{
  cmdset_.erase(cmd); 
}

void PyNetwork::setAllEventRX(bool state)
{
  rxall_ = state; 

}


void PyNetwork::rxEventsThread()
{
  // This function aggregates multiple reads and timeouts across the 
  // thread interface to produce something a bit more useful. 
  // 
  // It runs in its own thread
  
  int pipe = network_.getEventFifoPipe(); 
  
  int TIMELATENCYUS = 200000; // 200 ms; 
  
  while (true ) {
    timeval lastSend; 
    gettimeofday(&lastSend, NULL); 
    
    while(true) {
      
      fd_set readfds; 
      FD_SET(pipe, &readfds); 
      timeval timeout; 
      timeout.tv_sec = 0; 
      timeout.tv_usec = 100000; 
      

      select(pipe + 1, &readfds, NULL, NULL, &timeout); 
      if (FD_ISSET(pipe, &readfds)) {
	// we received an eventlist
	char buf[1000]; 
	int n = read(pipe, buf, 1000); 

	for (int i = 0; i < n; i++)
	  {
	    // extract out events
	    EventList_t * pel = network_.getNewEvents();
	    for (EventList_t::iterator ei = pel->begin(); 
		 ei != pel->end(); ei++ )

	      {
		Event_t e = *ei; 
		if (acceptEvent(e)) {
		  PyEvent_t pe; 
		  pe.cmd = e.cmd; 
		  pe.src = e.src; 
		  for (int eword = 0; eword < EVENTLEN-1; eword++) {
		    pe.data[eword] = e.data[eword];
		  }

		  boost::mutex::scoped_lock lock(pyEventListMutex_); 
		  
		  pyEventList_.push_back(pe); 
		  pyEventListCnt_++; 

		}
		
	      }
	    delete pel; 
	    
	  }

      }
    
      // check if it's been long enough
      timeval now; 
      gettimeofday(&now, NULL); 
      int elapsedtime_us = (now.tv_sec - lastSend.tv_sec)*1000000 + 
	(now.tv_usec - lastSend.tv_usec); 
      if (elapsedtime_us > TIMELATENCYUS) {

	break; 
      }
      
      
    }
    // now we update the cond variable to notify the other thread
    // that data is available
    eventsAvailable_.notify_one(); 
    
  }
}
	  


BOOST_PYTHON_MODULE(network)
{

  PyEval_InitThreads();

  def("greet", greet);
  
  
  class_<PyNetwork,  boost::noncopyable>("Network",
		    init<std::string>())
    .def_readwrite("name", &PyNetwork::name_)
    .def("run", &PyNetwork::run)
    .def("getEvents", &PyNetwork::getEvents)
    .def("enableEventRX", &PyNetwork::enableEventRX)
    .def("disableEventRX", &PyNetwork::disableEventRX)
    .def("setAllEventRX", &PyNetwork::setAllEventRX); 

  
  class_<PyEvent_t>("Event")
    .def_readwrite("cmd", &PyEvent_t::cmd)
    .def_readwrite("src", &PyEvent_t::src); 

    
}

