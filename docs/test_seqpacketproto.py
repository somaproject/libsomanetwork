from seqpacketproto import *

from nose.tools import *

def test_inrange():

    assert_true(inrange(0, 0, 1, 10))
    assert_true(inrange(1, 0, 1, 10))
    assert_false(inrange(2, 0, 1, 10))
    assert_raises(OverflowError, inrange, 11, 0, 1, 10)
    
    assert_true(inrange(8, 7, 2, 10))
    assert_true(inrange(9, 7, 2, 10))
    assert_true(inrange(10, 7, 2, 10))
    assert_true(inrange(0, 7, 2, 10))
    assert_true(inrange(1, 7, 2, 10))
    assert_true(inrange(2, 7, 2, 10))
    assert_false(inrange(3, 7, 2, 10))
    


def run_sequential_tst(seqIDlist, seqIDlistRX, SEQMAX):
    """
    Create a trivial test to see if we can recover
    the packet sequence ID list correctly, in
    order.
    
    """
    spp = SequentialPacketProtocol(SEQMAX, 2, 2, 10)

    data = [("packet %d" % id, id) for id in seqIDlist]
    for d in data:
        spp.addPacket(d[0], d[1])
        
    # can we read them out:
    recovereddata = spp.getCompletedPackets()
    print "recovered", recovereddata

    correctdata = [("packet %d" % id, id) for id in seqIDlistRX]
    recoveredvalid = [(d[0], d[1]) for d in recovereddata if d[1] != None]
    assert_equal(len(recoveredvalid), len(correctdata))

    for d, p in zip(correctdata, recoveredvalid):
        assert_equal(d[1], p[0])
        assert_equal(d[0], p[1])
        
    return spp

def test_PacketSequence():
    SEQMAX = 10
    p = PacketSequence(SEQMAX)

    # simple test
    p.append("zero", 0)
    p.append("one", 1)
    p.append("two", 2)

    assert_equal(p.headID, 0)
    assert_equal(p.tailID, 2)
    
    p.append("three", 3)
    assert_equal(p.tailID, 3)
    
    assert_equal(p.data, [(0, "zero"),
                          (1, "one"),
                          (2, "two"),
                          (3, "three")])
    p.replace("twoprime", 2)
    
    assert_equal(p.data, [(0, "zero"),
                          (1, "one"),
                          (2, "twoprime"),
                          (3, "three")])



def test_linear_addition():
    """
    Add packets in correct, sequential order
    """
    SEQMAX = 4
    d = [i % SEQMAX for i in range(20)]
    run_sequential_tst(d, d, SEQMAX)
    
    d = [i % SEQMAX for i in range(20, 30)]
    run_sequential_tst(d, d, SEQMAX)

def test_out_of_order():
    """
    add packets with one out of order
    """
    SEQMAX = 12
    indata = [0, 1, 3, 2, 4, 5]
    outdata = [0, 1, 2, 3, 4, 5]
    
    run_sequential_tst(indata, outdata, SEQMAX)
    
    indata = [0, 1, 3, 2, 4, 5, 7, 6]
    outdata = [0, 1, 2, 3, 4, 5, 6, 7]
    
    run_sequential_tst(indata, outdata, SEQMAX)

    # now with wrap-around
    indata = [9, 10, 0, 11, 1, 2]
    outdata = [9, 10, 11, 0, 1, 2]
    run_sequential_tst(indata, outdata, SEQMAX)
    
def test_dupe():
    """
    Test duplicate packets
    """

    SEQMAX = 12
    indata = [0, 1, 2, 2, 3, 4, 5]
    outdata = [0, 1, 2, 3, 4, 5]
    
    spp = run_sequential_tst(indata, outdata, SEQMAX)
    assert_equal(len(spp.dupes), 1)
    
    indata = [0, 1, 2, 3, 4, 2,  5]
    outdata = [0, 1, 2, 3, 4, 5]
  
    run_sequential_tst(indata, outdata, SEQMAX)

    indata = [0, 1, 2, 3, 0, 1, 2, 3, 4,  5]
    outdata = [0, 1, 2, 3, 4, 5]
  
    run_sequential_tst(indata, outdata, SEQMAX)

    
def test_out_of_order_dupe():
    """
    Test duplicate packets
    """
    SEQMAX = 12
    indata = [0, 1, 2, 4, 2, 3, 2, 5]
    outdata = [0, 1, 2, 3, 4, 5]
  
    run_sequential_tst(indata, outdata, SEQMAX)

def test_retx():
    """
    See if we correctly issue a retransmission request
    
    """

    SEQMAX = 12
    indata = [0, 1, 2, 3, 5, 6, 7, 4, 8, 9, 10, 11]
    outdata = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11]
  
    spp = run_sequential_tst(indata, outdata, SEQMAX)
    rrq =  spp.getRetransmitRequests()
    assert_equal(rrq, [4])

def test_retx_lost_single():
    """
    See if we correctly issue a retransmission request and then give up
    """

    SEQMAX = 25
    indata = [0, 1, 2, 3, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14]
    outdata = [0, 1, 2, 3, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14]
  
    spp = run_sequential_tst(indata, outdata, SEQMAX)
    rrq =  spp.getRetransmitRequests()
    assert_equal(rrq, [4])

def test_retx_lost_multiple():
    """
    See if we correctly issue multiple retx reqs and give up on them
    """

    SEQMAX = 25
    indata = [0, 1, 2, 3, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16]
    outdata = [0, 1, 2, 3, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16]
  
    spp = run_sequential_tst(indata, outdata, SEQMAX)
    rrq =  spp.getRetransmitRequests()
    assert_equal(rrq, [4, 5])
    

def test_random_permutation_packets():
    """
    Randomly permute a string of packets and see if we get
    the result
    """

    SEQMAX = 30

    indata = [0, 3, 2, 1, 4, 6, 5, 7,  7, 9, 1, 2, 8]

    outdata = range(10)
    
    spp = run_sequential_tst(indata, outdata, SEQMAX)
    

def test_noise_packets():
    """
    Randomly permute a string of packets and see if we get
    the result
    """

    SEQMAX = 1000

    indata = [0, 1, 2, 3, 200,  4, 201, 202, 203, 5, 400,  6, 7, 8, 9]

    outdata = range(10)
    
    spp = run_sequential_tst(indata, outdata, SEQMAX)
    

def test_abort_sequnce():
    """
    Test if we correctly abort a sequence
    """
    SEQMAX = 1000
    indata = [0, 1, 2, 3, 100, 101, 102, 103, 104, 105, 106, 107, 108,
              109, 110, 111]

    outdata = [0, 1, 2, 3, 100, 101, 102, 103, 104, 105, 106, 107, 108,
               109, 110, 111]
    spp = run_sequential_tst(indata, outdata, SEQMAX)
        

def test_abort_sequnce_2():
    """
    Test if we correctly abort a sequence, with junk in the middle
    
    """
    SEQMAX = 1000
    indata = [0, 1, 2, 3, 50, 100, 101, 102, 103, 104, 105, 106, 107, 108,
              109, 110, 111, 112, 113, 114, 115, 116,
               117, 118, 119]

    outdata = [0, 1, 2, 3, 50, 100, 101, 102, 103, 104, 105, 106, 107, 108,
               109, 110, 111, 112, 113, 114, 115, 116,
               117, 118, 119]

    
    spp = run_sequential_tst(indata, outdata, SEQMAX)
        

def test_abort_sequnce_3():
    """
    Test if we correctly abort a sequence, with junk in the middle
    and some noise packets that live in FUTURE
    """
    SEQMAX = 1000
    indata = [0, 1, 2, 3, 200, 201, 300, 301, 50, 100, 101, 102, 103, 104, 105, 106, 107, 108,
              109, 110, 111, 112, 113, 114, 115, 116,
               117, 118, 119]

    outdata = [0, 1, 2, 3, 50, 100, 101, 102, 103, 104, 105, 106, 107, 108,
               109, 110, 111, 112, 113, 114, 115, 116,
               117, 118, 119]

    
    spp = run_sequential_tst(indata, outdata, SEQMAX)
        

def test_overlapping_seq():
    SEQMAX = 1000
    indata = [0, 12, 1, 2, 3, 5, 6, 8, 4, 7, 8, 9, 10]

    outdata = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
    spp = run_sequential_tst(indata, outdata, SEQMAX)
    assert_equal(len( spp.packetSeqs), 1)
    
