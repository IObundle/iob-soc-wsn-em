#include "interconnect.h"
#include "txrx.h"

// Memory map
#include "txrx_mem_map.h"

//base address
int txrx;

void txrx_init(int base_address) {
  // capture base address
  txrx = base_address;
}

void tx_set_data(char value) {
  IO_SET(txrx, TX_DATA, (int)value);
}

void tx_set_start(char value) {
  IO_SET(txrx, TX_START, (int)value);
}

void tx_set_en(char value) {
  IO_SET(txrx, TX_EN, (int)value);
}

int tx_ready(void) {
  return (IO_GET(txrx, TX_READY));
}

void txrx_set_aa(int value) {
  IO_SET(txrx, TXRX_AA, value);
}

void txrx_set_ch_idx(char value) {
  IO_SET(txrx, TXRX_CH_IDX, (int)value);
}

void rx_set_start(char value) {
  IO_SET(txrx, RX_START, (int)value);
}

void rx_set_en(char value) {
  IO_SET(txrx, RX_EN, (int)value);
}

void rx_set_nb_pkg(char value) {
  value += 3; // add 3 bytes for CRC
  IO_SET(txrx, RX_NB_PKG, (int)value);
}

int rx_aa_found(void) {
  return (IO_GET(txrx, RX_AA_FOUND));
}

int rx_crc_valid(void) {
  return (IO_GET(txrx, RX_CRC_VALID));
}

int rx_empty(void) {
  return (IO_GET(txrx, RX_EMPTY));
}

int rx_data(void) {
  return (IO_GET(txrx, RX_DATA));
}

void demod_set_en(char value) {
  IO_SET(txrx, DEMOD_EN, (int)value);
}

