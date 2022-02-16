#pragma once
#pragma pack (1)

#include <stdlib.h>
#include <stdint.h>
#include "printf.h"

#define SN_ID 0
#define BS_ID 1

#define AA_LEN 4
#define PDU_H_LEN 2
#define ADV_IND_L_LEN 7
#define CONNECT_REQ_L_LEN 34
#define CRC_LEN 3
#define ZERO_PAYLOAD 0
#define MAX_PAYLOAD_LEN 37   

#define ADV_IND 0   	    
#define CONNECT_REQ 5			

typedef enum SN_MODES {
	MODE_SN_TX_ADV_IND=1,		
	MODE_SN_RX_CONNECT_REQ
} sn_modes_e_t;

typedef enum BS_MODES {
	MODE_BS_TX_CONNECT_REQ=11,	
	MODE_BS_RX_ADV_IND		
} bs_modes_e_t;

typedef struct SN {	  
    uint32_t  aa_init;     
    uint32_t  nextState;  
    uint8_t   id;    
} sn_s_t;
  
typedef struct BS {
    uint32_t aa_init;
    uint32_t nextState;
    uint32_t busyState;
    uint8_t  id;
} bs_s_t;

// Advertising PDU's header
typedef struct PDU_ADV_HEADER {
	uint16_t PDU_Type : 4,
		 RFU_1    : 2,
		 TxAdd    : 2,
		 RxAdd    : 1,
		 Length   : 6,
		 RFU_2    : 1;
} pdu_adv_h_s_t;

// Connectable undirected advertising PDU's payload
typedef struct ADV_IND_PAYLOAD {
	uint64_t AdvA    : 48,		        
		 AdvData : 8;	        
} adv_ind_payload_s_t;
 
typedef struct CONNECT_REQ_LLDATA {
	uint64_t  AA        : 32,	//4 bytes
		  CRCInit   : 24,       //3 bytes
		  WinSize   : 8,	//1 byte 
		  WinOffset : 16, 	//2 bytes
		  Interval  : 16,	//2 bytes
		  Latency   : 16, 	//2 bytes
		  Timeout   : 16, 	//2 bytes
		  ChM	    : 40, 	//5 bytes
		  Hop	    : 5, 	//5 bits 
	          SCA	    : 3; 	//3 bits 
} connect_req_lldata_s_t;

// Connect request PDU's payload
typedef struct CONNECT_REQ_PAYLOAD {
	uint64_t InitA : 48,			
		 AdvA  : 48;			
	connect_req_lldata_s_t LLData;	
} connect_req_payload_s_t;  
   
// Data PDU's header   
typedef struct PDU_DATA_HEADER { 
	uint16_t LLID 	: 2,		
		 NESN 	: 1,		
		 SN   	: 1,		
		 MD   	: 1,		
		 RFU  	: 3,		
		 Length : 8;		
} pdu_data_h_s_t;

// Data PDU's payload 
typedef struct PDU_LLControl_PAYLOAD {
	uint32_t  Opcode  : 8,  	
		  CtrData : 24;       
} pdu_llcontrol_payload_s_t;   
      

unsigned int get_size (unsigned char adv_or_data, unsigned char field_num);
