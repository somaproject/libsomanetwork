#ifndef PERIODICPATTERN_H
#define PERIODICPATTERN_H

class PeriodicPattern
{
  
public:

  int channel; //digital output channel number
  int period;  //cycle period in ecycles
  int offset;  //phase offset in ecycles
  double duty; //duty cycle in fraction of period

  //constructor
  PeriodicPattern(int, int,int,double);

};



#endif //PERIODICPATTERN_H
