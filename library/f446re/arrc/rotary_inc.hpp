#ifndef ARRC_ROTARY_INC_HPP
#define ARRC_ROTARY_INC_HPP
#include "mbed.h"

class RotaryInc {
public:
  RotaryInc(PinName pin_a, PinName pin_b, int resolution, int multi);
  ~RotaryInc();
  double getSpeed();
  double getDiff();
  double getSum();

private:
  InterruptIn *interrupt_a_, *interrupt_b_;
  Timer time_;
  double resolution_;
  int pulse_;
  int multi_;
  long sum_;
  void riseA(void);
  void riseB(void);
  void fallA(void);
  void fallB(void);
};
#endif