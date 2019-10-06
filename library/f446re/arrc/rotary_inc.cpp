#include "rotary_inc.hpp"

RotaryInc::RotaryInc(PinName pin_a, PinName pin_b, double diameter,
                     int resolution, int multi) {
  interrupt_a_ = new InterruptIn(pin_a, PullUp);
  interrupt_b_ = new InterruptIn(pin_b, PullUp);
  interrupt_a_->rise(callback(this, &RotaryInc::riseA));
  interrupt_b_->rise(callback(this, &RotaryInc::riseB));
  interrupt_b_->fall(callback(this, &RotaryInc::fallB));

  multi_ = multi;
  pulse_ = 0;
  sum_ = 0;
  if (multi_ == 2 || multi_ == 4) {
    interrupt_a_->fall(callback(this, &RotaryInc::fallA));
  } else {
    multi_ = 1;
  }

  time_ = new Timer;
  time_->start();
  resolution_ = resolution * multi_ / diameter;
}

void RotaryInc::riseA() {
  now_a_ = true;
  now_b_ ? --pulse_ : ++pulse_;
}

void RotaryInc::fallA() {
  now_a_ = false;
  now_b_ ? ++pulse_ : --pulse_;
}

void RotaryInc::riseB() {
  now_b_ = true;
  if (multi_ == 4) {
    now_a_ ? ++pulse_ : --pulse_;
  }
}

void RotaryInc::fallB() {
  now_b_ = false;
  if (multi_ == 4) {
    now_a_ ? --pulse_ : ++pulse_;
  }
}

double RotaryInc::getSpeed() {
  double speed = pulse_ / resolution_ / time_->read();
  time_->reset();
  pulse_ = 0;
  return speed;
}

double RotaryInc::diff() {
  double diff = pulse_ / resolution_;
  pulse_ = 0;
  return diff;
}

double RotaryInc::get() {
  sum_ += pulse_;
  pulse_ = 0;
  return sum_;
}

RotaryInc::~RotaryInc() {
  interrupt_a_->disable_irq();
  interrupt_b_->disable_irq();
  delete time_;
  delete interrupt_a_;
  delete interrupt_b_;
}
