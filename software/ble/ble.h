#pragma once

#include <stdlib.h>
#include <stdint.h>

#include "system.h"
#include "periphs.h"
#include "printf.h"

// Access Address length is 4 bytes
#define AA_LEN 4

// PDU's Header length is 2 bytes
#define PDU_H_LEN 2

// CRC sequence length is 3 bytes
#define CRC_LEN 3

// Zero size payload - used temporarily
#define ZERO_PAYLOAD 0

// Max Payload length = 37 bytes
#define MAX_PAYLOAD_LEN 37 

// Packet type 0x00 - Advertising/Connectable undirected advertising packet
#define ADV_IND 0

// Packet type 0x05 - Advertising/Connection request packet 
#define CONNECT_REQ 5			

// Connectable undirected advertising event - Advertising/First time connection
#define MODE_SN_TX_ADV_IND 1

// BS is free and can receive advertising packets
#define MODE_BS_RX_ADV_IND 3

// BS requests a conection
#define MODE_BS_TX_CONNECT_REQ 4

// SN receives BS connection request
#define MODE_SN_RX_CONNECT_REQ 2

#define SN_ID 0
#define BS_ID 1

typedef struct SN {
    uint8_t   id;	  
    uint32_t  aa_init;     // Initial Access Address
    uint32_t  nextState;      
} sn_t;
  
typedef struct BS {
    uint8_t  id;
    uint32_t aa_init;
    uint32_t nextState;
} bs_t;

// ADV_IND packet header
typedef struct PDU_ADV_HEADER {
	uint16_t pdu_type : 4,     //4 bits
		 rfu_1    : 2,	   //2 bits
		 tx_addr  : 1,	   //1 bit
		 rx_addr  : 1,	   //1 bit
		 length   : 6,	   //6 bits
		 rfu_2    : 2;     //2 bits
} pdu_adv_h_t;

// ADV_IND packet payload
typedef struct ADV_IND_PAYLOAD {
	uint64_t adv_device_a;	// advertiser device address
	uint64_t adv_data;	// advertising data from the advertiser
} adv_ind_payload_t;

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

