#include <mbed.h>
#include <rotary_inc.hpp>
#include <scrp_slave.hpp>

ScrpSlave slave(PA_9, PA_10, PA_12, SERIAL_TX, SERIAL_RX, 0x0803e000);

constexpr int MAXPWM = 250;
constexpr int PERIOD 256;
constexpr PinName USER_PIN[5][3] = {
    {PB_0, PB_1, PB_3},
    {PA_1, PA_3, PB_4},
    {PA_8, PA_7, PB_5},
    {PB_6, PA_11, PB_7},
    {PA_0, PA_4, NC} //ロータリーエンコーダー用
};

// RotaryInc rotary(rotarypin[0],rotarypin[1],10,200,0);

bool Drive(int id, int pwm) {
  pwm = constrain(pwm, -MAXPWM, MAXPWM);
  DigitalOut Led(pwmpin[id][2]);
  if (!pwm) {
    DigitalOut Moter1(pwmpin[id][0], 0);
    DigitalOut Moter2(pwmpin[id][1], 0);
    Led.write(0);
  } else if (0 < pwm) {
    PwmOut Moter1(pwmpin[id][0]);
    Moter1.period_us(Period);
    Moter1.write((float)pwm / 255);
    DigitalOut Moter2(pwmpin[id][1], 0);
    Led.write(1);
  } else {
    DigitalOut Moter1(pwmpin[id][0], 0);
    PwmOut Moter2(pwmpin[id][1]);
    Moter2.period_us(Period);
    Moter2.write((float)-pwm / 255);
    Led.write(1);
  }
  return true;
}

bool safe(int rx_data, int &tx_data) {
  for (int i = 0; i < 5; i++) {
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
