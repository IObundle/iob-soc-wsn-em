#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "system.h"
#include "periphs.h"

#ifndef SIM_SYNTH
#include "printf.h"
#endif

#define N_SHIFT(DATA_IN, N) ((DATA_IN >> 8*N) & 0xff) 

#define ITER                29 

#define MAX_N_SN            10
#define MAX_N_BYTES         39
#define MAX_N_DATA_CHANNELS 37
#define N_USED_DATA_CHANNELS 3
#define ADV_CH_FIRST        37
#define ADV_CH_SECOND       38
#define ADV_CH_LAST         39
#define SAMPLING_RATE	     4
#define MD_VAL_ONE	     1
#define MD_VAL_ZERO	     0    

#define ADV_H_LEN            2
#define LL_DATA_H_LEN        2
#define LL_DATA_TMP_P_LEN    4
#define ZERO_PAYLOAD	     0

#define PREAMBLE_LEN	     1  
#define AA_LEN               4 
#define CRC_LEN              3

//SN TX - Configuration of a connectable directed advertising packet's header
//random device addresses
//RFU_2 temporarily set for debugging purpose 			  
#define ADV_DIRECT_IND      1 
#define ADV_DIND_H_RFU_1    0
#define ADV_DIND_H_TxAdd    1    			
#define ADV_DIND_H_RxAdd    1
#define ADV_DIND_P_LEN     12
#define ADV_DIND_H_RFU_2    1 

//BS TX - Configuration of a connection request packet's header  
#define CONNECT_REQ         5			
#define CONNECT_REQ_H_RFU_1 0
#define CONNECT_REQ_H_TxAdd 1
#define CONNECT_REQ_H_RxAdd 1
#define CONNECT_REQ_P_LEN  34	 
#define CONNECT_REQ_H_RFU_2 1

//LLID - Start of an L2CAP message or a complete L2CAP message with no fragmentation
#define LL_DATA_PDU_SC      2
//LLID - Continuation fragment of an L2CAP message or an Empty PDU
#define LL_DATA_PDU_CE      1

//Time slot in us
#define T_Slot            625

//Inter Frame Space in us
#define T_IFS             150

//Advertising time on one adv channel
#define T_ADV           10000

//Advertising delay in us - should be within the range of 0-10ms
#define advDelay        10000
 
//Scan window in us
#define T_ScanWindow(N) N*T_Slot

//Scan interval in us
#define T_ScanInterval(M,N) M*T_ScanWindow(N)   

//Packet Time in us with bit rate is 1 Mbps - to be reviewed plus_T_IFS
#define T_PACKET(H,P) 8*(PREAMBLE_LEN + AA_LEN + H + P + CRC_LEN)  

//Connection timing 
#define W_MIN   1250
#define T_RX   10000 

//Transmit Window offset in us
#define transmitWindowOffset(WinOffset) (WinOffset * W_MIN)

//The transmit window starts at (transmitWindowOffset + 1.25ms) after the end of the CONNECT_REQ PDU
#define startTransmitWindow(WinOffset) (transmitWindowOffset(WinOffset) + W_MIN)

//Used data channels indices 
static uint8_t dataChIdx[N_USED_DATA_CHANNELS]={5, 22, 34};

typedef enum {
	FALSE=0,
	TRUE=1
} boolean_e_t;		
	
#pragma pack (1)
//-------------------------------------------------- 
//Advertising PDU's header
typedef struct PDU_ADV_HEADER {
	uint16_t PDU_Type : 4,
		 RFU_1    : 2,
		 TxAdd    : 1,
		 RxAdd    : 1,
		 Length   : 6, 
		 RFU_2    : 2;
} pdu_adv_h_s_t;
//-------------------------------------------------- 
//Connectable directed advertising PDU's payload
typedef struct ADV_DIRECT_IND_PAYLOAD {
	uint64_t AdvA  : 48,      
		 InitA : 48;          
} adv_direct_payload_s_t;

//Connectable directed advertising PDU
typedef struct ADV_DIRECT_IND_PDU {
	pdu_adv_h_s_t           h;
	adv_direct_payload_s_t  payload; 
} adv_direct_pdu_s_t;
//-------------------------------------------------- 
//Connect request PDU's payload
typedef struct CONNECT_REQ_PAYLOAD {
	uint64_t InitA            : 48,		//6 bytes	
		 AdvA             : 48,		//6 bytes
		 LLData_AA        : 32,         //4 bytes
		 LLData_CRCInit   : 24,	        //3 bytes
		 LLData_WinSize   : 8,      	//1 byte 
		 LLData_WinOffset : 16,     	//2 bytes
		 LLData_Interval  : 16,     	//2 bytes
		 LLData_Latency   : 16,     	//2 bytes
		 LLData_Timeout   : 16,     	//2 bytes
		 LLData_ChM	  : 40,         //5 bytes
		 LLData_Hop	  : 5,          //5 bits 
	         LLData_SCA	  : 3;          //3 bits		 		 		 		
} connect_req_payload_s_t;  
   
//Connect request PDU
typedef struct CONNECT_REQ_PDU {
	pdu_adv_h_s_t           h;
	connect_req_payload_s_t payload; 
} connect_request_pdu_s_t;   
//--------------------------------------------------  
//LL Data PDU's header   
typedef struct PDU_LLDATA_HEADER { 
	uint16_t LLID 	: 2,		
		 NESN 	: 1,		
		 SN   	: 1,		
		 MD   	: 1,		
		 RFU  	: 3,		
		 Length : 8;		
} pdu_lldata_h_s_t;
//-------------------------------------------------- 
//LL TMP Data PDU
typedef struct PDU_LLDATA_TMP {
	pdu_lldata_h_s_t    h;
	int32_t	            payload;
} pdu_lldata_tmp_s_t;  
//--------------------------------------------------      
#pragma pack ()

//Functions' prototypes
void sys_init(void); 
unsigned int get_adv_ch_freq(unsigned short adv_ch_idx); 
unsigned int get_data_ch_freq(unsigned short data_ch_idx);
void wp_set_ch_index(unsigned short ch_idx);
void wp_set_aa(unsigned int aa);
void sensor_node();
void base_station();
void print_data(uint64_t data_in, uint32_t size);
uint64_t gen_dataChMap(uint8_t dataCh[], unsigned int size);
