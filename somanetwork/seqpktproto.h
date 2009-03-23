#ifndef SOMANETWORK_SEQPKTPROTO_H
#define SOMANETWORK_SEQPKTPROTO_H

#include "somanetwork/packetsequence.h"
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/utility.hpp>

namespace somanetwork { 

  inline seqid_t seq_add(seqid_t x, seqid_t y, seqid_t SEQMAX) {
    return (x + y) % SEQMAX; 
  }

  inline seqid_t seq_sub(seqid_t x, seqid_t y, seqid_t SEQMAX) {
    /*
      Perform x - y
      That is, return the value that is y units earlier in the 
      sequence stream than X
      
     */ 
    if (x >= y) {
      return x - y; 
    }
    
    // otherwise 
    return (x + (SEQMAX - y )); 
    
  }


  inline seqid_t seq_dist(seqid_t x, seqid_t y, seqid_t SEQMAX) {
    /*
      Perform x - y

      We adopt the convetion that x > y in the circular coordinate system      

     */ 
    if (x >= y) {
      return x - y; 
    }
    
    // otherwise 
    return (x + (SEQMAX - y )); 
    
  }


  
  inline bool inrange(seqid_t id, seqid_t start, seqid_t end, seqid_t SEQMAX)
  {
    /*
      Assunme start and end define a range which wraps around 
      
    */
    if (id >= SEQMAX) {
      throw std::runtime_error("ID not within SEQMAX");       
    }

    seqid_t dist = seq_sub(end, start, SEQMAX); 
    seqid_t iddist = seq_sub(id, start, SEQMAX); 
    if (iddist <= dist) {
      return true;
    }

    return false; 

  }


  template<class T>
  class SequentialPacketProtocol : boost::noncopyable
  {
  public:
    SequentialPacketProtocol(seqid_t SEQMAX); 
    SequentialPacketProtocol(seqid_t SEQMAX, 
			     uint32_t RETX_REQ_THOLD, 
			     uint32_t RETX_WAIT_THOLD, 
			     uint32_t LOST_CAUSE_LEN, 
			     uint32_t LOST_CAUSE_THOLD); 

    void addPacket(T, seqid_t); 
    
    typedef std::list<T> outqueue_t; 
    
    outqueue_t getCompletedPackets(); 
    
    std::list<seqid_t> getRetransmitRequests(); 
    
    bool inFuture(seqid_t); 
    bool inPast(seqid_t); 
    
    int seqCount(); 

  private:
    const seqid_t SEQMAX_;   
    
    // Thresholds
    const int32_t RETX_REQ_THOLD; 
    const int32_t RETX_WAIT_THOLD; 
    const int32_t LOST_CAUSE_LEN; 
    const int32_t LOST_CAUSE_THOLD; 
    
    seqid_t currentID_; 
    bool waitingForFirst_; 

    int dupeCount_; 

    seqid_t retxReqWaiting_; 
    int retxReqWaitingDuration_; 
    outqueue_t outQueue_; 
    std::list<seqid_t> retxReq_; 

    typedef PacketSequence<T> packetsequence_t; 
    typedef typename  boost::ptr_map<seqid_t,packetsequence_t > pktSeqMap_t;
    typedef typename pktSeqMap_t::iterator pktSeqMapIterator_t; 

    pktSeqMap_t packetSequences_;

    bool canAppendToExistingSequence(seqid_t id); 
    seqid_t appendToExistingSequence(T val, seqid_t id); 
    void createNewSequence(T val, seqid_t id); 
    void consumeSequences(); 
    void checkLostCause(packetsequence_t & ps); 
    
    pktSeqMapIterator_t findNextPacketSequence(); 
    
  }; 
  
  template<class T>
  SequentialPacketProtocol<T>::SequentialPacketProtocol(seqid_t SEQMAX) : 
    SEQMAX_(SEQMAX), 
    RETX_REQ_THOLD(2), 
    RETX_WAIT_THOLD(2), 
    LOST_CAUSE_LEN(5), 
    LOST_CAUSE_THOLD(10), 
    currentID_(0), 
    waitingForFirst_(true), 
    retxReqWaiting_(0), 
    retxReqWaitingDuration_(-1), 
    dupeCount_(0)
  {


  }
    

  template<class T>
  SequentialPacketProtocol<T>::SequentialPacketProtocol(seqid_t SEQMAX, 
							uint32_t RETX_REQ_THOLD, 
							uint32_t RETX_WAIT_THOLD, 
							uint32_t LOST_CAUSE_LEN, 
							uint32_t LOST_CAUSE_THOLD):
    SEQMAX_(SEQMAX), 
    RETX_REQ_THOLD(RETX_REQ_THOLD), 
    RETX_WAIT_THOLD(RETX_WAIT_THOLD), 
    LOST_CAUSE_LEN(LOST_CAUSE_LEN), 
    LOST_CAUSE_THOLD(LOST_CAUSE_THOLD), 
    currentID_(0), 
    waitingForFirst_(true),
    retxReqWaiting_(0), 
    retxReqWaitingDuration_(-1), 
    dupeCount_(0)
  {
    
    
  }
  
  template<class T>
  bool SequentialPacketProtocol<T>::inPast(seqid_t id) {
    seqid_t half = SEQMAX_ / 2; 
    seqid_t past_end = seq_sub(currentID_, half, SEQMAX_); 
    return inrange(id, past_end, currentID_, SEQMAX_); 

  }

  template<class T>
  bool SequentialPacketProtocol<T>::inFuture(seqid_t id) {
    seqid_t half = SEQMAX_ / 2; 
    seqid_t futures_end = seq_add(currentID_, half, SEQMAX_); 
    return inrange(id, currentID_, futures_end, SEQMAX_); 


  }

  
  template<class T>
  void SequentialPacketProtocol<T>::addPacket(T val, seqid_t id)
  {
    if(retxReqWaitingDuration_ >= 0) {
      if (id == retxReqWaiting_) {
	retxReqWaitingDuration_ = -1; 
	retxReqWaiting_ = 0; 
      } else {
	retxReqWaitingDuration_++;       
      }
    }
    
    std::cout << "Add packet " << id 
	      << " with currentID = " << currentID_ 
	      << " with retxReqWaitingDuration_ = " << retxReqWaitingDuration_
	      << std::endl;
    // if first packet, just pass on
    if (waitingForFirst_){
      currentID_ = id; 
      outQueue_.push_back(val); 
      waitingForFirst_ = false; 
      return; 
    } 

    if (id == seq_add(currentID_, 1, SEQMAX_)) {
       // correct next packet
      std::cout << "Correct next packet " << std::endl;
      outQueue_.push_back(val); 
      currentID_ = id; 

    } else if (inPast(id) ) {
      // duplicate packet
      dupeCount_++; 
      
    } else if (inFuture(id)) {
      // In the future

      if(canAppendToExistingSequence(id)) {
	seqid_t existid = appendToExistingSequence(val, id);
	pktSeqMapIterator_t sid = packetSequences_.find(existid); 
	
	checkLostCause(*(sid->second)); 
      } else { 
	createNewSequence(val, id); 
      }
    }

    consumeSequences(); 
    // Now check for retransmission
    if (retxReqWaitingDuration_ > RETX_WAIT_THOLD) {
      std::cout << "retx abort" << std::endl; 
      // insert empty/missing // FIXME? 
      currentID_ = seq_add(currentID_, 1, SEQMAX_); 
      consumeSequences(); 
      retxReqWaiting_ = 0; 
      retxReqWaitingDuration_ = -1; 
    } else { 
      if (retxReqWaitingDuration_ == -1) {
	// not in the middle of a retx
	typename SequentialPacketProtocol<T>::pktSeqMapIterator_t pi = findNextPacketSequence(); 
	if (pi != packetSequences_.end()) {
	  if (pi->second->size() >= RETX_REQ_THOLD) {
	    std::cout << "retx send" << std::endl; 
	    seqid_t retxid = seq_add(currentID_, 1, SEQMAX_); 

	    retxReq_.push_back(retxid); 
	    retxReqWaiting_ = retxid; 
	    retxReqWaitingDuration_ = 0; 	  
	  }
	}
      }
    }
  }
    
  template<class T>
  std::list<T> SequentialPacketProtocol<T>::getCompletedPackets()
  {
    outqueue_t outcopy; 
    outcopy = outQueue_; 
    outQueue_.clear(); 
    
    return outcopy; 
  }
    
  template<class T>
  std::list<seqid_t> SequentialPacketProtocol<T>::getRetransmitRequests()
  {
    std::list<seqid_t> retxcpy;
    retxcpy = retxReq_; 

    retxReq_.clear(); 
    return retxcpy; 


  }

  template<class T>
  bool SequentialPacketProtocol<T>::canAppendToExistingSequence(seqid_t id)
  {
    // FIXME LINEAR TIME
    typedef typename pktSeqMap_t::iterator pi_t; 
    for(pi_t pi = packetSequences_.begin(); pi != packetSequences_.end(); ++pi) {
      if(seq_add(pi->second->tailID(), 1, SEQMAX_) == id) {
	return true; 
      }
    }
    return false; 
  }

  template<class T>
  seqid_t SequentialPacketProtocol<T>::appendToExistingSequence(T val, seqid_t id)
  {
    typedef PacketSequence<T> packetsequence_t; 
    typedef typename  boost::ptr_map<seqid_t,packetsequence_t > pktSeqMap_t;

    typedef typename pktSeqMap_t::iterator pi_t; 
    
    for(pi_t pi = packetSequences_.begin(); pi != packetSequences_.end(); ++pi) {
      if(seq_add(pi->second->tailID(), 1, SEQMAX_) == id) {
	pi->second->append(val, id); 
	return pi->first; 
      }
    }
    
  }

  template<class T>
  void SequentialPacketProtocol<T>::createNewSequence(T val, seqid_t id)
  {
    packetSequences_.insert(id, new packetsequence_t(val, id, SEQMAX_)); 

  }


  template<class T>
  void SequentialPacketProtocol<T>::consumeSequences()
  {
    //packetSequences_.insert(id, new packetsequence_t(val, id, SEQMAX_)); 
    seqid_t nextid = seq_add(currentID_, 1, SEQMAX_); 
    typedef typename pktSeqMap_t::iterator pi_t; 
    pi_t pi = packetSequences_.find(nextid); 
    while (pi != packetSequences_.end() ){
      std::cout << "Consuming sequence with " 
		<< pi->second->headID() << " " << pi->second->tailID() << std::endl; 

      // sequence has nextId at its head
      std::list<T> & pkts = pi->second->packets(); 
      typedef typename std::list<T>::iterator iter_t; 
      for(iter_t i = pkts.begin(); i != pkts.end(); i++) {
	outQueue_.push_back(*i); 
      }
      currentID_ = pi->second->tailID(); 

      packetSequences_.erase(nextid); 
      pi = packetSequences_.find(nextid); 
      
    }
    
  }

  template<class T>
  void SequentialPacketProtocol<T>::checkLostCause(packetsequence_t & ps) {
    seqid_t dist = seq_dist(ps.headID(), currentID_, SEQMAX_); 
    typedef typename std::list<T> tlist_t; 
    if((ps.size() > LOST_CAUSE_LEN) and 
       (dist > LOST_CAUSE_THOLD)) 
      {
	typedef typename pktSeqMap_t::iterator pi_t; 
	std::cout << "Calling lost cause with dist = " << dist
		  << " using packet sequence (" 
		  << ps.headID() << ", " << ps.tailID() 
		  << ") as the sequence"  << std::endl; 
	
	while(true) { 
	  pi_t pi; 
	  std::cout << "---------------------------------\n"; 
	  std::cout << "In this loop, ps.headid = " 
		    << ps.headID() << std::endl; 
	    

	  pi = packetSequences_.lower_bound(currentID_); 
	  
	  if (pi == packetSequences_.end()) { 
	    std::cout << "packetSequences pi = end" << std::endl; 
	    if (ps.headID() < currentID_) {
	      // try wrap-around
	      currentID_ = 0; 
	    }  else {
	      break; 
	    }
	  } else {
	    std::cout << "packetSequences pi != end" << std::endl; 
	    std::cout << "pi->second->headID() = " 
		      << pi->second->headID() 
		      << " ps.headID = " << ps.headID() 
		      << "currentID = " << currentID_ << std::endl; 
	    if (ps.headID() == pi->second->headID())
	      {
		std::cout << "This is our own packet seq"
			  << std::endl; 
		break; 
	      }
	    

	    // found something; 

	    if (currentID_ < ps.headID() ) { 
	      // currentID and ps have correct sequencing
	      
	      if (pi->second->headID() > ps.headID()) {
		break; 
	      } else {
		// consume
		std::cout << "Appending sequence with " 
			  << pi->second->headID() << " to "  
			  << pi->second->tailID() << std::endl; 

		tlist_t & pktlist = pi->second->packets(); 
		
		for(typename tlist_t::iterator i = pktlist.begin(); i != pktlist.end(); ++i)
		  {
		    outQueue_.push_back(*i); 
		  }
		currentID_ = pi->second->tailID(); 
		packetSequences_.erase(pi->second->headID()); 
	      }
	      
	    } else {
	      // this is clearly a valid packet: 
	      tlist_t & pktlist = pi->second->packets(); 
	      for(typename tlist_t::iterator i = pktlist.begin(); i != pktlist.end(); ++i)
		{
		  outQueue_.push_back(*i); 
		}
	      currentID_ = pi->second->tailID(); 
	      packetSequences_.erase(pi->second->headID()); 
	      
	      
	    }
	    
	    
	  }
	  
	}
	// now add the current packet sequence
	std::cout << "Adding the new lost cause packet head" 
		  << " head = " << ps.headID() << " tail = " 
		  << ps.tailID() << std::endl; 
	tlist_t & pktlist = ps.packets(); 
	for(typename tlist_t::iterator i = pktlist.begin(); i != pktlist.end(); ++i)
	  {
	    outQueue_.push_back(*i); 
	  }
	currentID_ = ps.tailID();
	packetSequences_.erase(ps.headID()); 
	retxReqWaitingDuration_ = -1; 
	retxReqWaiting_ = 0; 
	
      }
  }


  template<class T>
  int SequentialPacketProtocol<T>::seqCount()
  {
    return packetSequences_.size(); 
    
  }

  template<class T>
  typename SequentialPacketProtocol<T>::pktSeqMapIterator_t SequentialPacketProtocol<T>::findNextPacketSequence()
  {
    /*
      Circular find
      
    */
    pktSeqMapIterator_t pi; 

    pi = packetSequences_.lower_bound(currentID_); 
    if (pi != packetSequences_.end()) {
      return pi; 
      
    } else { 
      pi = packetSequences_.lower_bound(0); 
      return pi;       
    }

  }


}

#endif // SOMANETWORK_SEQPKTPROTO_H
