
Overview
============

The SequentialPacketProtocol is a container / protocol interface
for our sequential packet transmission. 

We assume that our data source is transmitting packets
with sequential packet IDs, n, n+1, n+2, etc. This wraps
around at some value, we'll call it SEQMAX.

The system also has the possibility of sending previously-transmitted
packets -again-, within the same datastream, if you ask for them.
This is accomplished by you requesting the packet with a particular
SequenceID. Sometimes, packet n will then be transmitted [as is often
the case if only a short time period has elapsed]. Other times,
however, a totally unrelated packet my be retransmitted with a
sequence number that is arbitrarially different from the requested n.

The challenge is to address the following network-related conditions: 

   1. out-of-order packet delivery. This looks like
      1 2 3 4 6 5 7 8 9 

   2. Duplicate transmission. This looks like: 
      1 2 3 4 5 6 3 7 8 9 
   
      The packets sent in response to a retransmission request
      can be viewed as a special case of duplicate transmission, 
      on nodes which successfully received the original packet. 

   3. Total crack-pot packets -- due to either corruption or the 
      retransmission interface sending old junk. 

Our goals are :
   1. best-effort in-order packet delivery
   2. handle these noise packets
   3. handle network disconnection gracefully


Implementation
============================
A "PacketSequence" is a sequential list of packets and their associated
SequenceIDs. 

The "CurrentID" is the ID of the most-recently-put-in-output-queue
packet. That is, the most recent "good packet" that's passed out
of the container. 

SEQMAX defines the maximum range of packet values. We use
this value to define two useful packet ranges: 

   FUTURE: The range of packets from CurrentID to ((CurrentID + SEQMAX/2) % SEQMAX

   PAST: The range of packets between CurrentID and ((CurrentID - SEQMAX/2) % SEQMAX
  
In some sense, FUTURE represents the range of packets we could reasonably
expect to see in the future, and PAST are packets we've already seen
and committed. This is a heuristic, and not a sure-fire solution --
in particular, if packets are dropped for SEQMAX/2, then
the future becomes the past, and the universe explodes, or we get 
confused. 

We define several thresholds: 

RETX_REQ_THOLD
   The number of packets we wait before issuing a retransmission request

RETX_WAIT_THOLD
   The number of packets we wait until declariing an individual packet lost

LOST_CAUSE_THOLD
   The delta between CurrentID and the next-closest

LOST_CAUSE_LEN 
   The length of a sequence that allows it to be considered for "lost cause"
   status


Algorithm
---------

Our receive algorithm is as follows: 

With new packet P:
   if P.id == (CurrentID + 1 ) % SEQMAX:
      # this is the next packet, add to the output queue
      CurrentID = P.id
   else:
      if P.id in PAST: 
         # ignore the packet
      	 increment "dupe" counter
	 # largely ignore
      if P.id in FUTURE:
         are there any pending PacketSequences that can take this
	 packet? That is, can we agppend this to the tail
	 of any packet sequences? 
            Yes-> do it
            No -> create new packet sequence
   # now examine the packet sequences: 

   While (next PacketSequence in order starts with CurrentID + 1)? 
      yes, commit that packet sequence
      update CurrentID

   # dealing with dropped packets
   if len(next PacketSequence) > RETX_THOLD:
      request CurrenTID + 1    
      set "Requested " == true
   
   # if len(next PacketSequence) > LOST_THOLD: (say, 5):
     Add "lost packet" into stream for currentID +1
     CurrentID++ 
     request ReTX of this packet?   sure
   
   # if len(next PacketSequence) > RESET_SEQUENCE: 
     reset the packet sequence
     Basically, this is the worst-case, "give up" code path


   
Questions we still lack good answers to: 

what if we observe the following: 

1 2 3 50 51 52 100 101 102 103 104 [...]

What is the correct behavior? 

  clearly we should output a sequence like 1 2 3 50 51 52 ...

  if 50-3 exceeds our threshold for "likely retransmission", we should
  abort... 

  But the take-home message is that when we abort-to-new-sequence, it 
  might not always be to the first sequence in the list

so we get a new packet
  we add it to a sequence
  if that sequence is > N and if it is too far from the past: 
     abort. 

