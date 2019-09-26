#ifndef ARRC_PID_HPP
#define ARRC_PID_HPP
#include <mbed.h>

namespace arrc {
class PidPosition {
public:
  PidPosition(double p, double i, double d, double max_i) {
    setParam(p, i, d, max_i);
    time_.start();
  }
  void setParam(double p, double i, double d, double max_i) {
    p_gain_ = p;
    i_gain_ = i;
    d_gain_ = d;
    max_i_control_ = max_i;
  }
  double control(double goal, double out) {
    delta_t_ = time_.read();
    time_.reset();
    prev_error_ = current_error_;
    current_error_ = goal - out;
    p_item_ = p_gain_ * current_error_;
    i_item_ += i_gain_ * current_error_ * delta_t_;
    if (i_item_ > max_i_control_) {
      i_item_ = max_i_control_;
    } else if (i_item_ < -max_i_control_) {
      i_item_ = -max_i_control_;
    }

    d_item_ = d_gain_ * (current_error_ - prev_error_) / delta_t_;
    control_ = p_item_ + i_item_ + d_item_;
    return control_;
  }
  /* ~PidPosition() { delete time_; } */

private:
  double p_gain_, i_gain_, d_gain_;
  double p_item_, i_item_ = 0, d_item_;
  Timer time_;
  double delta_t_;
  double control_ = 0;
  double current_error_ = 0, prev_error_ = 0, prev_prev_error_ = 0;
  double max_i_control_ = 0;
};

class PidVelocity {
public:
  PidVelocity(double p, double i, double d, double max_i) {
    setParam(p, i, d, max_i);
    time_ = new Timer;
    time_->start();
  }
  void setParam(double p, double i, double d, double max_i) {
    p_gain_ = p;
    i_gain_ = i;
    d_gain_ = d;
    max_i_control_ = max_i;
  }
  double control(double goal, double out) {
    delta_t_ = time_->read();
    time_->reset();
    prev_prev_error_ = prev_error_;
    prev_error_ = current_error_;
    current_error_ = goal - out;
    double p_item = p_gain_ * (current_error_ - prev_error_);
    double i_item = i_gain_ * current_error_ * delta_t_;
    if (i_item > max_i_control_) {
      i_item = max_i_control_;
    } else if (i_item < -max_i_control_) {
      i_item = -max_i_control_;
    }

    double d_item =
        d_gain_ *
        ((current_error_ - prev_error_) - (prev_error_ - prev_prev_error_)) /
        delta_t_;
    control_ += p_item + i_item + d_item;
    return control_;
  }
  ~PidVelocity() { delete time_; }

private:
  double p_gain_, i_gain_, d_gain_;
  Timer *time_;
  double delta_t_;
  double control_ = 0;
  double current_error_ = 0, prev_error_ = 0, prev_prev_error_ = 0;
  double max_i_control_ = 0;
};
} // namespace arrc

#endif
