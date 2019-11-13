#ifndef ARRC_ROTARY_INC_HPP
#define ARRC_ROTARY_INC_HPP
#include <mbed.h>

class RotaryInc {
public:
  RotaryInc(PinName userA, PinName userB, double circumference, int Resolution,
            int mode = 1); //速度計測有効
  ~RotaryInc();
  long long get();
  double getSpeed();
  void reset();
  double diff();

private:
  InterruptIn *A, *B;
  Timer time;
  long long pulse;
  long long last[20];
  long long prev;
  int spend;
  int count;
  int mode;
  int Resolution;
  double now;
  double sum;
  double pre_t[20];
  double speed;
  double circumference;
  bool measur;
  bool startflag;
  bool flag;
  void init(PinName, PinName);
  void riseA(void);
  void riseB(void);
  void fallA(void);
  void fallB(void);
  void calcu(void);
  void zero(void);
};
#endif
