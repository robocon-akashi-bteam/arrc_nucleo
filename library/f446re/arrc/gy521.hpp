#pragma once
#include "mbed.h"

const double GY521_LSB_MAP[4] = {131, 65.5, 32.8, 16.4};
enum GY521RegisterMap {
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

class GY521 {
public:
  GY521(I2C &i2c, int bit = 2, int calibration = 1000, double user_reg = 1.0);
  ~GY521();
  double yaw;
  double diffyaw;
  // double temp;
  void updata();
  double checkStatus(int mode);

private:
  I2C &i2c;
  Timer time;
  unsigned int dev_id;
  int16_t gyroRead2(enum GY521RegisterMap reg);
  double gyro_z_aver;
  double gyro_z_now;
  double gyro_z_prev;
  double gyro_LSB;
  int bit_;
  int calibration;
};