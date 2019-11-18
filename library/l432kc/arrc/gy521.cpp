#include "gy521.hpp"

Gy521::Gy521(uint8_t dev_id, uint8_t bit, double user_reg)
    : i2c_(PB_9, PB_8), dev_id_(dev_id), bit_(bit), user_reg_(user_reg) {
  send_id_ = dev_id << 1;
  if ((readByte(Gy521RegisterMap(WHO_AM_I)) & 0x7E) == 0x68) {
    if (readByte(Gy521RegisterMap(PWR_MGMT_1)) == 0x40) {
      writeByte(Gy521RegisterMap(PWR_MGMT_1), 0x00);
    }
    found_dev = true;
  }
  if (found_dev) {
    writeByte(Gy521RegisterMap(FS_SEL), bit_ << 3);
    writeByte(Gy521RegisterMap(AFS_SEL), 0x00);
  }
}

void Gy521::calibration(int calibration_time) {
  yaw = diff_yaw = 0;
  if (found_dev) {
    int16_t accel_x_now = 0, accel_y_now = 0, accel_z_now = 0;
    double accel_x_aver = 0, accel_y_aver = 0, accel_z_aver = 0;
    for (int i = 0; i < calibration_time; ++i) {
      accel_x_now = gyroRead2(ACCEL_XOUT_H);
      accel_y_now = gyroRead2(ACCEL_YOUT_H);
      accel_z_now = gyroRead2(ACCEL_ZOUT_H);
      accel_x_aver += accel_x_now;
      accel_y_aver += accel_y_now;
      accel_z_aver += accel_z_now;
    }
    accel_x_aver /= calibration_time;
    accel_y_aver /= calibration_time;
    accel_z_aver /= calibration_time;
    double gyro_reg =
        hypot(hypot(accel_x_aver, accel_y_aver), accel_z_aver) / accel_z_aver;
    gyro_LSB_ = GY521_LSB_MAP[bit_] / gyro_reg / user_reg_;

    int16_t gyro_z_now;
    gyro_z_aver_ = 0;
    for (int i = 0; i < calibration_time; ++i) {
      gyro_z_now = gyroRead2(GYRO_ZOUT_H);
      gyro_z_aver_ += gyro_z_now;
    }
    gyro_z_aver_ /= calibration_time;

    time.start();
    finished_calibration = true;
  }
}

void Gy521::update() {
  if (!finished_calibration) {
    return;
  }
  int16_t gyro_z_now = gyroRead2(GYRO_ZOUT_H);
  gyro_z_now = ((double)gyro_z_now - gyro_z_aver_) / gyro_LSB_;
  diff_yaw = (gyro_z_now + gyro_z_prev_) / 2 * time.read();
  time.reset();
  yaw += diff_yaw;
  gyro_z_prev_ = gyro_z_now;
  // temp = (double)gyroRead2(TEMPERATURE)/340+36.53;
  if (yaw > 180) {
    yaw -= 360;
  } else if (yaw <= -180) {
    yaw += 360;
  }
}

double Gy521::checkStatus(int mode) {
  if (mode == 0) {
    return gyro_LSB_;
  } else if (mode == 1) {
    return gyro_z_aver_;
  } else if (mode == 2) {
    return acos(gyro_LSB_ / GY521_LSB_MAP[bit_]);
  }
  return 0;
}
