
"""
This is some prototype code to test our algorithm  / scheme for packet
retransmission and recovery. Development is so much easier in The Hiss. 

"""
import numpy as np

class PacketSequence(object):
    def __init__(self, SEQMAX):
        self.headID = 0
        self.tailID = 0
        self.data = []
        self.SEQMAX = SEQMAX
        
    def append(self, P, id):
        if len(self.data) == 0:
            # first packet
            self.headID = id
            self.tailID = id
            self.data.append((id, P))
        else:
            assert id == (self.tailID + 1) % self.SEQMAX
            self.data.append((id, P))
            self.tailID = id

    def replace(self, P, id):
        if not self.contains(id):
            raise LookupError
        
        for i, (pid, p) in enumerate(self.data):
            if pid == id:
                self.data[i] = (id, P)
                break

    def contains(self, id):
        """
        check if this packet sequence contains this id
        """
        return inrange(id, self.headID, self.tailID, self.SEQMAX)


class SequentialPacketProtocol(object):
    """
    Sequential Packet Protocol
    """

    def __init__(self, SEQMAX, RETX_REQ_THOLD = 2, RETX_WAIT_THOLD = 2,
                 LOST_CAUSE_LEN = 5,
                 LOST_CAUSE_THOLD=10):
        self.SEQMAX = SEQMAX

        self.currentID = None

        self.packetSeqs = {}

        self.outqueue = []
        self.dupes = []
        self.retxReq = []
        self.RETX_REQ_THOLD = RETX_REQ_THOLD
        self.RETX_WAIT_THOLD = RETX_WAIT_THOLD
        self.LOST_CAUSE_THOLD = LOST_CAUSE_THOLD
        self.LOST_CAUSE_LEN = LOST_CAUSE_LEN
        self.retxReqWaiting = None
        self.retxReqWaitingDuration = 0
        
    def inpast(self, id):
        p  = (self.currentID - self.SEQMAX/2) % self.SEQMAX
        return inrange(id, p, self.currentID, self.SEQMAX)

    def infuture(self, id):

        f  = (self.currentID + self.SEQMAX/2) % self.SEQMAX
        return inrange(id, self.currentID, f, self.SEQMAX)

    def addPacket(self, P, id):
        """
        add new packet P
        """
        print "Adding packet", id, "----------------------------------------"

        if self.retxReqWaiting != None:
            self.retxReqWaitingDuration += 1
            
        if self.currentID == None:
            self.currentID = id
            self.outqueue.append((id, P))
            return

        if id  == (self.currentID + 1) % self.SEQMAX:
            # correct next packet
            print "id = %d is the correct next packet" % id
            self.currentID = (self.currentID + 1) % self.SEQMAX
            self.outqueue.append((id, P))

        elif self.inpast(id):
            print "id = %d is in the past" % id
            self.dupes.append((id, P))

        elif self.infuture(id):
            print "id = %d is in the future" % id
            # search the tails of existing sequences
            existing = False
            # does a == b 
            for headid, pktseq in self.packetSeqs.iteritems():
                tailid = pktseq.tailID
                if (tailid + 1) % self.SEQMAX == id:
                    # append to this packetseq
                    print "Appending id=%d to seq" % id
                    pktseq.append(P, id)
                    existing = True
                    if self.checkLostCause(pktseq):
                        return
                    
                elif pktseq.contains(id):
                    pktseq.replace(P, id)
                    existing = True

            if not existing:
                # create new packet sequence
                print "creating new PacketSequence with headid = %d" % id
                ps = PacketSequence(self.SEQMAX)
                ps.append(P, id)
                # insert into queue
                self.packetSeqs[id] = ps

        # now look at the sequences
        self.consumeSequences()
            
        # now we should check if there is any lost packet
        # stuff? This requires we have some concept of "next packet sequence"
        # which is a function of our current position

        # find first key in future:
        infuture_ids = [id for id in self.packetSeqs.keys() if self.infuture(id)]
        # normalize and sort
        infuture_ids_zeroed = [(id - self.currentID ) % self.SEQMAX for id in
                               infuture_ids]
        s = sorted(infuture_ids_zeroed)
        
        # handle and evaluate retransmission
        if len(s) > 0:
            seqid = (s[0] + self.currentID) % self.SEQMAX
            assert seqid in self.packetSeqs
            seq = self.packetSeqs[seqid]
            print "The length of the sequence is", len(seq.data), self.LOST_CAUSE_THOLD

            if self.retxReqWaitingDuration > self.RETX_WAIT_THOLD:
                print "The retransmission request for %d has exceeded the time we are willing to wait" %  self.retxReqWaiting
                self.outqueue.append((self.currentID + 1, None))
                self.currentID = (self.currentID + 1) % self.SEQMAX
                self.consumeSequences()
                self.retxReqWaiting = None
                self.retxReqWaitingDuration = 0
                
            elif len(seq.data) > self.RETX_REQ_THOLD:
                retxid = (self.currentID +1) % self.SEQMAX
                if self.retxReqWaiting != retxid:
                    print "issuing retransmission request for %d" % retxid
                    self.retxReq.append(retxid)
                    self.retxReqWaiting = retxid
                    self.retxReqWaitingDuration = 0
                    

    def checkLostCause(self, seq):
        """
        Checks if we're in a 'lost cause' scenario,
        where we believe:
           1. this sequence represents the new, current state and
           2. there's no point in trying to make up the packets between
              the currentID and the front of this sequence

        """
        if len(seq.data) > self.LOST_CAUSE_LEN and \
               ((seq.headID  - self.currentID) % self.SEQMAX) > self.LOST_CAUSE_THOLD:
            print "The pending data is a lost cause, we're setting the sequence"
            print "beginning with %d as our new head"

            # consume all sequences between here and there
            pktseqs_ids = sorted(self.packetSeqs.keys())
            future_ids = [id for id in pktseqs_ids if self.infuture(id)]

            centered_ids = sorted([(id - self.currentID) % self.SEQMAX
                            for id in future_ids])
            seqid_centered = (seq.headID - self.currentID)  % self.SEQMAX
            print "centered_ids = ", centered_ids, seqid_centered
            
            for id in centered_ids:
                if id >= seqid_centered:
                    break
                else:
                    realid = (id + self.currentID) % self.SEQMAX
                    seq = self.packetSeqs[realid]
                    del self.packetSeqs[realid]
                    self.outqueue += seq.data

            self.currentID = (seq.headID - 1) % self.SEQMAX

            self.retxWaiting = None
            self.retxWaitingDuration = 0
            
            return True
        
        return False
    def consumeSequences(self):
        nextid = (self.currentID +1) % self.SEQMAX
        while nextid in self.packetSeqs:
            print "A sequence has %d as its head" % nextid
            seq = self.packetSeqs[nextid]

            del self.packetSeqs[nextid]
            self.outqueue += seq.data
            self.currentID = seq.tailID
            
            nextid = (seq.tailID + 1) % self.SEQMAX
        
    def getCompletedPackets(self):
        """

        """
        l = list(self.outqueue)
        self.outqueue = []
        return l
    
        
    def getRetransmitRequests(self):
        """

        """
        s = list(self.retxReq)
        self.retxReq = []
        return s
    
def inrange(id, start, end, SEQMAX):
    """
    Check if the id is in the range defined by
    the circular interval [start, end]

    SEQMAX is the maximum-value
    
    """
    id_centered = (id - start) % SEQMAX
    end_centered = (end - start) % SEQMAX

    if id > SEQMAX:
        raise OverflowError
    
    if id_centered <= end_centered:
        return True
    
    return False
    
