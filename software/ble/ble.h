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
char ble_receive(char buffer[], unsigned char size); 

// Send data
char ble_send(char buffer[], unsigned char size);
