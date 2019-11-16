#include "scrp_slave.hpp"

constexpr int STX = 0x41;
constexpr int DMY = 0xff;

ScrpSlave::ScrpSlave(PinName tx_pin, PinName rx_pin, PinName rede_pin,
                     uint32_t baudrate, uint32_t flash_addr)
    : timeout_(timeout), serial_(tx_pin, rx_pin, baudrate), rede_(rede_pin) {
  mode_ = 1;
  /* serial_[0]->attach(callback(this, &ScrpSlave::port1), Serial::RxIrq); */
  if (flash_.init() == 0) {
    if (flash_.read(&id_, address_, 1) != 0) {
      id_ = 0;
    }
  } else {
    id_ = 0;
  }
  for (int i = 1; i <= MAX_NUM_CMD; ++i) {
    procs_[i] = NULL;
  }
}

void ScrpSlave::addCMD(uint8_t cmd,
                       bool (*proc)(int cmd, int rx_data, int &tx_data)) {
  if (cmd == 0 || cmd == 254)
    return;
  procs_[cmd] = proc;
}

void ScrpSlave::setTimeout(int time) { timeout_ = time; }

void ScrpSlave::changeID(uint8_t id) {
  flash_->erase(address_, flash_->get_sector_size(address_));
  flash_->program(&id, address_, 1);
}

int ScrpSlave::send(uint8_t id, uint8_t cmd, int16_t tx_data) {
  uint8_t tx_dataL = tx_data;
  uint8_t tx_dataH = tx_data >> 8;
  uint8_t tx_sum = id + cmd + tx_dataL + tx_dataH;

  const uint8_t data[] = {DMY, STX, id, cmd, tx_dataL, tx_dataH, tx_sum, DMY};
  if (!serial_[port]->writeable()) {
    return -1;
  }
  if (mode_ % 2 == 1 && id == 0) {
    rede_->write(1);
  }
  for (int i = 0; i < 8; i++) {
    serial_[port]->putc(data[i]);
  }
  while (!serial_[port]->writeable())
    ;
  if (mode_ % 2 == 1 && id == 0) {
    rede_->write(0);
  }

  int i = 0;
  bool received = false;
  bool stxflag = false;
  uint8_t rx[5] = {}, sum = 0;
  Timer out;
  out.start();
  while (out.read_ms() < timeout_ && !received) {
    while (serial_[port]->readable()) {
      if (serial_[port]->getc() == STX && !stxflag) {
        stxflag = true;
        continue;
      }
      if (stxflag) {
        rx[i] = serial_[port]->getc();
        sum += rx[i++];
      }
      if (i > 4) { /*
             uint8_t sum = 0;
             for(int j = 0;j<4;j++){
                 sum += rx[j];
             }*/
        if (sum == rx[4]) {
          received = true;
        }
        break;
      }
    }
  }
  out.stop();
  if (!received) {
    return -1;
  }
  return (rx[2] + (rx[3] << 8));
}

void ScrpSlave::update() {
  uint8_t rx_cmd;
  int16_t rx_data;
  bool received = false;
  bool broadcast = false;
  while (serial_[port]->readable()) {
    if (serial_[port]->getc() != STX)
      continue;
    uint8_t rx_id = serial_[port]->getc();
    uint8_t tmp_rx_cmd = serial_[port]->getc();
    uint8_t tmp_rx_dataL = serial_[port]->getc();
    uint8_t tmp_rx_dataH = serial_[port]->getc();
    uint8_t rx_sum = serial_[port]->getc();

    uint8_t sum = rx_id + tmp_rx_cmd + tmp_rx_dataL + tmp_rx_dataH;
    if (sum != rx_sum) {
      continue;
    }

    if (rx_id == 255) {
      broadcast = true;
    } else if (my_id_ == rx_id) {
      broadcast = false;
    } else {
      break;
    }

    rx_cmd = tmp_rx_cmd;
    rx_data = tmp_rx_dataL + ((int16_t)tmp_rx_dataH << 8);
    received = true;
  }
  if (!received) {
    return;
  }
  int tx_data = rx_data;
  if (rx_cmd == 0) {
    tx_data = rx_data;
  } else if (rx_cmd == 254) {
    uint8_t new_id = rx_data;
    my_id_ = new_id;
    changeID(new_id);
  } else if (procs_[rx_cmd] == 0 || !procs_[rx_cmd](rx_cmd, rx_data, tx_data)) {
    return;
  }
  if (broadcast) {
    return;
  }

  uint8_t tx_dataL = tx_data;
  uint8_t tx_dataH = tx_data >> 8;
  uint8_t tx_sum = my_id_ + rx_cmd + tx_dataL + tx_dataH;

  const uint8_t data[] = {DMY,      STX,      my_id_, rx_cmd,
                          tx_dataL, tx_dataH, tx_sum, DMY};
  if (!serial_[port]->writeable()) {
    return;
  }
  if (mode_ % 2 == 1 && port == 0) {
    rede_->write(1);
  }
  for (int i = 0; i < 8; i++) {
    serial_[port]->putc(data[i]);
  }
  while (!serial_[port]->writeable())
    ;
  if (mode_ % 2 == 1 && port == 0) {
    rede_->write(0);
  }
  return;
}

ScrpSlave::~ScrpSlave() {
  delete serial_[0];
  delete flash_;
  if (mode_ % 2 == 1) {
    delete rede_;
  }
  if (mode_ >= 2) {
    delete serial_[1];
  }
}
