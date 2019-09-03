#pragma once
#include "mbed.h"

class ScrpSlave {
public:
  ScrpSlave(PinName TX1, PinName RX1, uint32_t addr); // RedePinなし、１ポート
  ScrpSlave(PinName TX1, PinName RX1, PinName REDE1,
            uint32_t addr); // RedePinあり、１ポート
  ScrpSlave(PinName TX1, PinName RX1, PinName TX2, PinName RX2,
            uint32_t addr); // RedePinなし、２ポート
  ScrpSlave(PinName TX1, PinName RX1, PinName REDE1, PinName TX2, PinName RX2,
            uint32_t addr); // RedePinあり、１ポート＋RedePinなし、１ポート
  ~ScrpSlave();
  void setTimeout(int);
  void addCMD(uint8_t cmd, bool (*proc)(int rx_data, int &tx_data));
  int send(uint8_t id, uint8_t cmd, int16_t tx_data);
  int send2(uint8_t id, uint8_t cmd, int16_t tx_data);

private:
  DigitalOut *rede;
  Serial *serial[2];
  FlashIAP *flash;
  uint8_t mode;
  uint8_t my_id;
  uint32_t addr;
  int timeout;
  bool (*procs[256])(int rx_data, int &tx_data);
  int sending(int, uint8_t, uint8_t, int16_t);
  void changeID(uint8_t);
  void check(int port);
  void init(PinName, PinName);
  void port1();
  void port2();
};