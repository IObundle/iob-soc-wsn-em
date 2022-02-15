// Init BLE modules
void ble_init(void);

// Configure BLE
char ble_config(float channel_freq, int mode);

// Set Payload
void ble_payload(char nbytes);

// Turn BLE On as receiver
char ble_recv_on(void);

// Turn BLE On as sender
char ble_send_on(void);

// Turn BLE Off
char ble_off(void);

// Receive data
char ble_receive(char *buffer, char size);

// Send data
char ble_send(char *buffer, char size);

// Configure ADPLL and configure BLE for data transmission
void config_tx(float channel_freq);

// Configure payload size, configure ADPLL and configure BLE for data reception
void config_rx(unsigned int pdu_size, float channel_freq);
