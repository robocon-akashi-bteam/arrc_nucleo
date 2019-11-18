#ifndef ARRC_ROTARY_INC_HPP
#define ARRC_ROTARY_INC_HPP
#include <mbed.h>

class RotaryInc {
public:
  RotaryInc(PinName pin_a, PinName pin_b, int resolution, int multi = 1,
            double diameter = 1);
  ~RotaryInc();
  void update();
  void setSpeedBufferSize(int size) {
    speed_buffer_size_ = size;
    free(speed_buffer_);
    speed_buffer_ =
        (long long *)(malloc(sizeof(long long) * speed_buffer_size_));
  }
  double speed, sum, diff;
  long long raw_sum;

private:
  InterruptIn interrupt_a_, interrupt_b_;
  Timer time;
  double diameter_;
  int multi_, resolution_;
  long long pulse_, pulse_prev_;
  void riseA(void);
  void riseB(void);
  void fallA(void);
  void fallB(void);

  int speed_buffer_size_ = 1;
  long long *speed_buffer_ = NULL;
  int speed_buffer_head_, speed_buffer_tail_;
  void pushSpeedBuffer(long long data) {
    speed_buffer_tail_ = (speed_buffer_tail_ + 1) % speed_buffer_size_;
    speed_buffer_[speed_buffer_tail_] = data;

    if (speed_buffer_head_ == speed_buffer_tail_) {
      speed_buffer_head_ = (speed_buffer_head_ + 1) % speed_buffer_size_;
    }
  }
  long long getSpeedBuffer() { return speed_buffer_[speed_buffer_head_]; }
};

#endif
