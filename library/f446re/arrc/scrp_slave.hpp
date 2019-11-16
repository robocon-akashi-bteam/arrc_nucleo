#ifndef SCRP_SLAVE_H
#define SCRP_SLAVE_H
#include "mbed.h"

class ScrpSlave {
public:
  ScrpSlave(PinName TX1, PinName RX1, PinName REDE1, uint32_t baudrate,
            uint32_t timeout);
  ~ScrpSlave();
  void addCMD(uint8_t cmd, bool (*proc)(int cmd, int rx_data, int &tx_data));
  void update();

private:
  int send(uint8_t id, uint8_t cmd, int16_t tx_data);
  DigitalOut rede_;
  Serial serial_;
  FlashIAP flash_;
  uint8_t id_;
  static constexpr uint32_t address_ = ;
  uint32_t timeout_;
  static constexpr int MAX_NUM_CMD = 254;
  bool (*procs_[256])(int cmd, int rx_data, int &tx_data);
  void changeID(uint8_t);
};

#endif
