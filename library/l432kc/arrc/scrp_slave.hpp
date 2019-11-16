#ifndef SCRP_SLAVE_H
#define SCRP_SLAVE_H
#include "mbed.h"

class ScrpSlave {
public:
  ScrpSlave(PinName TX1, PinName RX1, PinName REDE1,
            uint32_t baudrate = 115200);
  ~ScrpSlave();
  void addCMD(uint8_t cmd, bool (*proc)(int cmd, int rx_data, int &tx_data));
  void update();
  void send(uint8_t tx_cmd, int tx_data);

private:
  DigitalOut rede_;
  Serial serial_;
  FlashIAP flash_;
  uint8_t id_;
  static constexpr uint32_t address_ = 0x0803e000;
  static constexpr int MAX_NUM_CMD = 256;
  bool (*procs_[MAX_NUM_CMD])(int cmd, int rx_data, int &tx_data);
  static constexpr uint8_t CHANGE_ID_CMD = 254;
  static constexpr int NUM_DATA = 8;
  static constexpr int STX = 0x41;
  static constexpr int DMY = 0xff;
  void changeId(uint8_t id);
};

#endif
