#include "scrp_slave.hpp"

ScrpSlave::ScrpSlave(PinName tx_pin, PinName rx_pin, PinName rede_pin,
                     uint32_t baudrate)
    : serial_(tx_pin, rx_pin, baudrate), rede_(rede_pin) {
  if (flash_.init() == 0) {
    if (flash_.read(&id_, address_, 1) != 0) {
      id_ = 0;
    }
  } else {
    id_ = 0;
  }
  for (int i = 1; i < MAX_NUM_CMD; ++i) {
    procs_[i] = NULL;
  }
}

void ScrpSlave::addCMD(uint8_t cmd,
                       bool (*proc)(int cmd, int rx_data, int &tx_data)) {
  if (cmd == 0 || cmd == CHANGE_ID_CMD) {
    return;
  }
  procs_[cmd] = proc;
}

void ScrpSlave::changeId(uint8_t id) {
  id_ = id;
  flash_.erase(address_, flash_.get_sector_size(address_));
  flash_.program(&id_, address_, 1);
}

void ScrpSlave::update() {
  uint8_t rx_cmd;
  int16_t rx_data;
  bool received = false;
  bool broadcast = false;
  while (serial_.readable()) {
    if (serial_.getc() != STX) {
      continue;
    }
    uint8_t rx_id = serial_.getc();
    uint8_t dummy_rx_cmd = serial_.getc();
    uint8_t dummy_rx_data_low = serial_.getc();
    uint8_t dummy_rx_data_high = serial_.getc();
    uint8_t rx_sum = serial_.getc();

    uint8_t sum = rx_id + dummy_rx_cmd + dummy_rx_data_low + dummy_rx_data_high;
    if (sum != rx_sum) {
      continue;
    }

    if (rx_id == 255) {
      broadcast = true;
    } else if (id_ == rx_id) {
      broadcast = false;
    } else {
      continue;
    }

    rx_cmd = dummy_rx_cmd;
    rx_data = dummy_rx_data_low + ((int16_t)dummy_rx_data_high << 8);
    received = true;
  }
  if (!received) {
    return;
  }

  int tx_data = rx_data;
  if (rx_cmd == 0) {
    tx_data = rx_data;
  } else if (rx_cmd == CHANGE_ID_CMD) {
    changeId(rx_data);
  } else if (procs_[rx_cmd] == NULL ||
             !procs_[rx_cmd](rx_cmd, rx_data, tx_data)) {
    return;
  }
  if (broadcast) {
    return;
  }

  send(rx_cmd, tx_data);
  return;
}

void ScrpSlave::send(uint8_t tx_cmd, int tx_data) {
  uint8_t tx_data_low = tx_data;
  uint8_t tx_data_high = tx_data >> 8;
  uint8_t tx_sum = id_ + tx_cmd + tx_data_low + tx_data_high;

  const uint8_t data[NUM_DATA] = {DMY,         STX,          id_,    tx_cmd,
                                  tx_data_low, tx_data_high, tx_sum, DMY};
  /* if (!serial_.writeable()) { */
  /*   return; */
  /* } */
  rede_.write(1);
  for (int i = 0; i < NUM_DATA; i++) {
    serial_.putc(data[i]);
  }
  while (!serial_.writeable())
    ;
  rede_.write(0);
  return;
}

ScrpSlave::~ScrpSlave() {}
