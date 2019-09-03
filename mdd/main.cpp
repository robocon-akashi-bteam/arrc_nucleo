#include <mbed.h>
#include <rotary_inc.hpp>
#include <scrp_slave.hpp>

ScrpSlave slave(PA_9, PA_10, PA_12, SERIAL_TX, SERIAL_RX, 0x0803e000);

constexpr int MAX_PWM = 250;
constexpr int PERIOD = 256;
constexpr PinName MOTOR_PIN[4][3] = {{PB_0, PB_1, PB_3},
                                     {PA_1, PA_3, PB_4},
                                     {PA_8, PA_7, PB_5},
                                     {PB_6, PA_11, PB_7}};
DigitalOut *motor_pwm[4];
DigitalOut *motor_led[4];
constexpr PiName ENCODER_PIN[4][2] = {
    {PA_0, PA_4},
    {PA_1, PA_3},
    {PA_8, PA_7},
    {PB_6, PA_11}
}

float map(float value, float from_low, float from_high, float to_low, float to_high) {
  return value * (to_high - to_low) / (from_high - from_low);
}

bool drive(int id, int pwm) {
  if (pwm != 0) {
    motor_pwm[id][0]->write(0);
    motor_pwm[id][1]->write(0);
    motor_led.write(0);
  } else if (0 < pwm) {
    pwm = map(pwm, -MAX_PWM, MAX_PWM, -1.0, 1.0);
    Moter1.period_us(Period);
    Moter1.write((float)pwm / 255);
    DigitalOut Moter2(pwmpin[id][1], 0);
    Led.write(1);
  } else {
    pwm = -map(pwm, -MAX_PWM, MAX_PWM, -1.0, 1.0);
    DigitalOut Moter1(pwmpin[id][0], 0);
    PwmOut Moter2(pwmpin[id][1]);
    Moter2.period_us(Period);
    Moter2.write((float)-pwm / 255);
    Led.write(1);
  }
  return true;
}

bool safe(int rx_data, int &tx_data) {
  for (int i = 0; i < 4; i++) {
    DigitalOut Moter1(pwmpin[i][0], 0);
    DigitalOut Moter2(pwmpin[i][1], 0);
    DigitalOut Led(pwmpin[i][2], 0);
  }
  return true;
}

bool DM1(int rx_data, int &tx_data) { return Drive(0, rx_data); }

bool DM2(int rx_data, int &tx_data) { return Drive(1, rx_data); }

bool DM3(int rx_data, int &tx_data) { return Drive(2, rx_data); }

bool DM4(int rx_data, int &tx_data) { return Drive(3, rx_data); }

int main() {
  Drive(0, 0);
  Drive(1, 0);
  Drive(2, 0);
  Drive(3, 0);
  slave.addCMD(2, DM1);
  slave.addCMD(3, DM2);
  slave.addCMD(4, DM3);
  slave.addCMD(5, DM4);
  slave.addCMD(255, safe);
  while (true) {
  }
}
