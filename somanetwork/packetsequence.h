#ifndef SOMANETWORK_PACKETSEQUENCE_H
#define SOMANETWORK_PACKETSEQUENCE_H
#include <stdint.h>
#include <list>

namespace somanetwork
{
  typedef uint64_t seqid_t; 

  template<class T>
  class PacketSequence
  {
  public:
    PacketSequence(T val, seqid_t id, seqid_t SEQMAX); 
    void append(T val, seqid_t id); 
    void replace(T val, seqid_t id); 
    bool contains(seqid_t id); 
    size_t size(); 
    seqid_t headID(); 
    seqid_t tailID(); 
    std::list<T> & packets(); 

  private:
    seqid_t headID_; 
    seqid_t tailID_; 
    const seqid_t SEQMAX_; 
    typedef std::list<T> packetseqlist_t; 
    packetseqlist_t packets_; 
    void checkEmpty(); 

  }; 
  
  template<class T>
  PacketSequence<T>::PacketSequence(T val, seqid_t id, seqid_t seqmax) :
    headID_(id),
    tailID_(id), 
    SEQMAX_(seqmax)
  {
    packets_.push_back(val); 
  }

  template<class T>
  void PacketSequence<T>::append(T val, seqid_t id)
  {
    if(id != (tailID_ + 1) % SEQMAX_) {
      throw std::runtime_error("attempted to append non-sequential packet sequence ID"); 
    }
    packets_.push_back(val); 
    tailID_ = id; 
  }

  template<class T>
  size_t PacketSequence<T>::size()
  {
    return packets_.size(); 
  }

  template<class T>
  void PacketSequence<T>::replace(T val, seqid_t id)
  {
    if(contains(id)) { 
      // Recenter
      seqid_t tail_centered = (tailID_ - headID_) % SEQMAX_; 
      seqid_t id_centered = (id - headID_) % SEQMAX_; 
      
      typename packetseqlist_t::iterator pi; 
      pi = packets_.begin(); 
      for(seqid_t i = 0; i < tail_centered; i++) {
	
	if (i == id_centered) {
	  *pi = val; 
	  break; 
	  
	}
	pi++; 
      }
      
    } else { 
      throw std::runtime_error("attempted to replace non-contained packet");
    }
    
  }


  template<class T>
  bool PacketSequence<T>::contains(seqid_t id)
  {
    seqid_t range = (tailID_ - headID_) % SEQMAX_; 
    if ((id - headID_) % SEQMAX_ <= range) {
      return true; 
    } else {
      return false;
    }

  }

  template<class T>
  seqid_t PacketSequence<T>::headID()
  {
    return headID_; 
  }

  template<class T>
  seqid_t PacketSequence<T>::tailID()
  {
    return tailID_; 
  }
  
  template<class T>
  std::list<T> & PacketSequence<T>::packets()
  {
    return packets_;     
  }

}

#endif // SOMANETWORK_PACKETSEQUENCE_H
