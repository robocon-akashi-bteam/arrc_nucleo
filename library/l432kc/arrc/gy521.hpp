#ifndef ARRC_GY521_HPP
#define ARRC_GY521_HPP
#include "mbed.h"

const double GY521_LSB_MAP[4] = {131, 65.5, 32.8, 16.4};
enum Gy521RegisterMap {
  WHO_AM_I = 0x75,
  PWR_MGMT_1 = 0x6B,
  LPF = 0x1A,
  FS_SEL = 0x1B,
  AFS_SEL = 0x1C,
  ACCEL_XOUT_H = 0x3B,
  ACCEL_YOUT_H = 0x3D,
  ACCEL_ZOUT_H = 0x3F,
  // TEMPERATURE  = 0x41,
  // GYRO_XOUT_H = 0x43,
  // GYRO_YOUT_H = 0x45,
  GYRO_ZOUT_H = 0x47
};

class Gy521 {
public:
  Gy521(uint8_t dev_id = 0x68, uint8_t bit = 2, double user_reg = 1.0);
  void calibration(int calibration_time);
  double yaw, diff_yaw;
  void update();
  double checkStatus(int mode);
  bool found_dev = false;

private:
  I2C i2c_;
  Timer time;
  uint8_t dev_id_, send_id_;
  uint8_t bit_;
  double user_reg_;
  double gyro_LSB_, gyro_z_aver_;
  int16_t gyro_z_prev_;
  bool finished_calibration = false;
  int16_t gyroRead2(uint8_t adress_high) {
    return (readByte(adress_high) << 8) + readByte(adress_high + 1);
  }
  uint8_t readByte(uint8_t address) {
    char command[1] = {};
    command[0] = address;
    i2c_.write(send_id_, command, 1, true);
    char data[1];
    i2c_.read(send_id_, data, 1);
    return data[0];
  }
  void writeByte(uint8_t address, uint8_t data) {
    i2c_.start();
    i2c_.write(send_id_);
    i2c_.write(address);
    i2c_.write(data);
    i2c_.stop();
  }
};
#endif
