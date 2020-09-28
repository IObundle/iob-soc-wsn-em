// Functions

// Initialize TXRX
void txrx_init(int base_address);

// Set TX data
void tx_set_data(char value) {
  IO_SET(txrx, TX_DATA, (int)value);
}

// Set TX start
void tx_set_start(char value) {
  IO_SET(txrx, TX_START, (int)value);
}

// Set TX enable
void tx_set_en(char value) {
  IO_SET(txrx, TX_EN, (int)value);
}

// Get TX ready
int tx_ready(void) {
  return (IO_GET(txrx, TX_READY));
}

// Set TXRX Access Address
void txrx_set_aa(int value) {
  IO_SET(txrx, TXRX_AA, value);
}

// Set TXRX Channel index
void txrx_set_ch_idx(char value) {
  IO_SET(txrx, TXRX_CH_IDX, (int)value);
}

// Set RX start
void rx_set_start(char value) {
  IO_SET(txrx, RX_START, (int)value);
}

// Set RX enable
void rx_set_en(char value) {
  IO_SET(txrx, RX_EN, (int)value);
}

// Get Access Address found
int rx_aa_found(void) {
  return (IO_GET(txrx, RX_AA_FOUND));
}

// Get CRC valid
int rx_crc_valid(void) {
  return (IO_GET(txrx, RX_CRC_VALID));
}

// Get RX empty
int rx_empty(void) {
  return (IO_GET(txrx, RX_EMPTY));
}

// Get RX data
int rx_data(void) {
  return (IO_GET(txrx, RX_DATA));
}

// Set DEMOD enable
void demod_set_en(char value) {
  IO_SET(txrx, DEMOD_EN, (int)value);
}

// Send data
#define send(value) tx_set_data(value)

// Receive data
#define receive() rx_data()

// Enable TX
#define tx_enable() tx_set_en(1)

// Disable TX
#define tx_disable() tx_set_en(0)

// Enable RX
#define rx_enable() rx_set_pd(1)

// Disable RX
#define rx_disable() rx_set_en(0)

// Enable DEMOD
#define demod_enable() demod_set_en(1)

// Disable DEMOD
#define demod_disable() demod_set_en(0)
