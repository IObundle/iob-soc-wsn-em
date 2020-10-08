#include "adpll.h"
#include "pa.h"
#include "mixer.h"
#include "lpf.h"
#include "limiter.h"
#include "txrx.h"
#include "iref.h"

// Init BLE modules
void ble_init(void);

// Turn BLE On as receiver
char ble_recv_on(void);

// Turn BLE On as sender
char ble_send_on(void);

// Turn BLE Off
char ble_off(void);

// Receive data
char ble_receive(char *buffer);

// Send data
char ble_send(char *buffer, char size);
