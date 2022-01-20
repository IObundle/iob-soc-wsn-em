//#include "stdlib.h"
#include "system.h"
#include "periphs.h"

#include "iob-uart.h"
#include "iob_timer.h"
#include "printf.h"

#include "adpll.h"
#include "pa.h"
#include "mixer.h"
#include "lpf.h"
#include "limiter.h"
#include "txrx.h"
#include "iref.h"

// Access Address length is 4 bytes
#define AA_LEN 4

// PDU's Header length is 2 bytes
#define PDU_H_LEN 2

// CRC sequence length is 3 bytes
#define CRC_LEN 3

// Packet type 0b0000 - An SN is looking to connect with the BS for the first time
#define ADV_IND 0

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
