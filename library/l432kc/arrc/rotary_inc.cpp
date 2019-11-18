#include "rotary_inc.hpp"

RotaryInc::RotaryInc(PinName pin_a, PinName pin_b, int resolution, int multi,
                     double diameter)
    : interrupt_a_(pin_a, PullUp), interrupt_b_(pin_b, PullUp),
      resolution_(resolution), multi_(multi), diameter_(diameter) {
  interrupt_a_.rise(callback(this, &RotaryInc::riseA));

  if (multi_ == 2) {
    interrupt_a_.fall(callback(this, &RotaryInc::fallA));
  } else if (multi_ == 4) {
    interrupt_a_.fall(callback(this, &RotaryInc::fallA));
    interrupt_b_.rise(callback(this, &RotaryInc::riseB));
    interrupt_b_.fall(callback(this, &RotaryInc::fallB));
  } else {
    multi_ = 1;
  }
  time.start();
  setSpeedBufferSize(1);
}

void RotaryInc::update() {
  raw_sum = pulse_;
  sum = (double)pulse_ / resolution_;
  diff = (double)(pulse_ - pulse_prev_) / resolution_;
  pushSpeedBuffer(pulse_);
  speed =
      (double)(pulse_ - getSpeedBuffer()) / (time.read() * speed_buffer_size_);

  pulse_prev_ = pulse_;
  time.reset();
}

void RotaryInc::riseA() { interrupt_b_.read() ? --pulse_ : ++pulse_; }

void RotaryInc::fallA() { interrupt_b_.read() ? ++pulse_ : --pulse_; }

void RotaryInc::riseB() { interrupt_a_.read() ? ++pulse_ : --pulse_; }

void RotaryInc::fallB() { interrupt_a_.read() ? --pulse_ : ++pulse_; }

RotaryInc::~RotaryInc() {
  interrupt_a_.disable_irq();
  interrupt_b_.disable_irq();
}