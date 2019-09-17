#include "rotary_inc.hpp"

RotaryInc::RotaryInc(PinName pin_a, PinName pin_b, int resolution, int multi) {
  interrupt_a_ = new InterruptIn(pin_a, PullUp);
  interrupt_b_ = new InterruptIn(pin_b, PullUp);

  multi_ = multi;
  pulse_ = 0;
  sum_ = 0;
  interrupt_a_->rise(callback(this, &RotaryInc::riseA));
  if (multi_ == 2) {
    interrupt_a_->fall(callback(this, &RotaryInc::fallA));
  } else if (multi_ == 4) {
    interrupt_a_->fall(callback(this, &RotaryInc::fallA));
    interrupt_b_->rise(callback(this, &RotaryInc::riseB));
    interrupt_b_->fall(callback(this, &RotaryInc::fallB));
  } else {
    multi_ = 1;
  }

  time_.start();
  resolution_ = resolution * multi_;
}

void RotaryInc::riseA() { interrupt_b_->read() ? --pulse_ : ++pulse_; }

void RotaryInc::fallA() { interrupt_b_->read() ? ++pulse_ : --pulse_; }

void RotaryInc::riseB() { interrupt_a_->read() ? ++pulse_ : --pulse_; }

void RotaryInc::fallB() { interrupt_a_->read() ? --pulse_ : ++pulse_; }

double RotaryInc::getSpeed() {
  double speed = pulse_ / resolution_ / time_.read();
  time_.reset();
  pulse_ = 0;
  return speed;
}

double RotaryInc::getDiff() {
  double diff = pulse_ / resolution_;
  pulse_ = 0;
  return diff;
}

double RotaryInc::getSum() {
  sum_ += pulse_;
  pulse_ = 0;
  return sum_ / resolution_;
}

RotaryInc::~RotaryInc() {
  interrupt_a_->disable_irq();
  interrupt_b_->disable_irq();
  delete interrupt_a_;
  delete interrupt_b_;
}
