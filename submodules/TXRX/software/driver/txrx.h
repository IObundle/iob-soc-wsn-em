// Functions

// Initialize TXRX
void txrx_init(int base_address);

// Set TX data
void tx_set_data(char value);

// Set TX start
void tx_set_start(char value);

// Set TX enable
void tx_set_en(char value);

// Get TX ready
int tx_ready(void);

// Set TXRX Access Address
void txrx_set_aa(int value);

// Set TXRX Channel index
void txrx_set_ch_idx(char value);

// Set RX start
void rx_set_start(char value);

// Set RX enable
void rx_set_en(char value);

// Get Access Address found
int rx_aa_found(void);

// Get CRC valid
int rx_crc_valid(void);

// Get RX empty
int rx_empty(void);

// Get RX data
int rx_data(void);

// Set DEMOD enable
void demod_set_en(char value);

// Send data
#define send(value) tx_set_data(value)

// Receive data
#define receive() rx_data()

// Enable TX
#define tx_enable() tx_set_en(1)

// Disable TX
#define tx_disable() tx_set_en(0)

// Enable RX
#define rx_enable() rx_set_en(1)

// Disable RX
#define rx_disable() rx_set_en(0)

// Enable DEMOD
#define demod_enable() demod_set_en(1)

// Disable DEMOD
#define demod_disable() demod_set_en(0)

// TX On
#define tx_on() tx_enable()

// RX On
#define rx_on() ({\
      rx_enable();\
      demod_enable();\
    })

// TX Off
#define tx_off() tx_disable()

// RX Off
#define rx_off() ({\
      rx_disable();\
      demod_disable();\
    })

// TXRX Off
#define txrx_off() ({\
      tx_off();\
      rx_off();\
    })
