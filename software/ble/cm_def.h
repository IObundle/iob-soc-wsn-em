#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "system.h"
#include "periphs.h"

#ifdef DBUG
#include "printf.h"
#endif

#define ITER  12

#define MAX_N_SN            10
#define MAX_N_BYTES         39
#define MAX_N_DATA_CHANNELS 37
#define ADV_CH_FIRST        37

#define ADV_H_LEN          2
#define LL_DATA_H_LEN      2
#define LL_DATA_GPS_P_LEN 18
#define LL_DATA_TMP_P_LEN  4
#define LL_CONTROL_P_LEN   4  
#define CRC_LEN            3

//SN TX - Configuration of a connectable directed advertising packet's header
//random device addresses
//RFU_2 temporarily set for debugging purpose 			  
#define ADV_DIRECT_IND   1 
#define ADV_DIND_H_RFU_1 0
#define ADV_DIND_H_TxAdd 1    			
#define ADV_DIND_H_RxAdd 1
#define ADV_DIND_P_LEN  12
#define ADV_DIND_H_RFU_2 1 

//BS TX - Configuration of a connection request packet's header  
#define CONNECT_REQ         5			
#define CONNECT_REQ_H_RFU_1 0
#define CONNECT_REQ_H_TxAdd 1
#define CONNECT_REQ_H_RxAdd 1
#define CONNECT_REQ_P_LEN  34	 
#define CONNECT_REQ_H_RFU_2 1

//LLID - LL Control PDU
#define LL_CONTROL_PDU 3
//LLID - Start of an L2CAP message or a complete L2CAP message with no fragmentation
#define LL_DATA_PDU_SC 2
//LLID - Continuation fragment of an L2CAP message or an Empty PDU
#define LL_DATA_PDU_CE 1

//Opcode - New - used to build the ACK packet
#define LL_CONNECTION_ACK 23
//Opcode - Remote User Terminated Connection
#define LL_TERMINATE_IND 2

//CtrlData
#define POSITIVE_ACK 1
#define END_CONNECTION 19

//Standby delay in us - temporarily setting 
#define t_standby 500

//Inter Frame Space in us
#define T_IFS 150

//Advertising delay in us
#define advDelay 10000

//Scan Window in us
#define scanWindow 10000
	
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
	pdu_adv_h_s_t           pdu_adv_ind_h;
	adv_direct_payload_s_t  pdu_adv_ind_payload; 
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
	pdu_adv_h_s_t           pdu_adv_ind_h;
	connect_req_payload_s_t connect_req_payload; 
} connect_request_pdu_s_t;   
//--------------------------------------------------  
//LL Control and LL Data PDU's header   
typedef struct PDU_LLDATA_HEADER { 
	uint16_t LLID 	: 2,		
		 NESN 	: 1,		
		 SN   	: 1,		
		 MD   	: 1,		
		 RFU  	: 3,		
		 Length : 8;		
} pdu_lldata_h_s_t;
//-------------------------------------------------- 
//LL Control PDU's payload 
typedef struct PDU_LLControl_PAYLOAD {
	uint32_t  Opcode  : 8,  	
		  CtrData : 24;       
} pdu_llcontrol_payload_s_t;  
 
//ACK PDU
typedef struct PDU_LLControl_ACK {
    pdu_lldata_h_s_t	      pdu_lldata_h;		
    pdu_llcontrol_payload_s_t pdu_llcontrol_payload;
} pdu_llcontrol_s_t;  

//Terminate Connection PDU
typedef struct PDU_LLControl_TERMINATE {
    pdu_lldata_h_s_t	      pdu_lldata_h;		
    pdu_llcontrol_payload_s_t pdu_llcontrol_payload;
} pdu_llcontrol_terminate_s_t; 
//-------------------------------------------------- 
typedef struct GPS_DATA {
	uint64_t DMS_Lat_D : 32,
		 DMS_Lat_M : 16,
		 DMS_Lat_S : 16,   //not a floating number for now	
		 DMS_Lat_C : 8,
		 DMS_Lng_D : 32,
		 DMS_Lng_M : 16,
		 DMS_Lng_S : 16,   //not a floating number for now
		 DMS_Lng_C : 8;
} gps_coordinates_s_t;
//-------------------------------------------------- 
//LL GPS Data PDU
typedef struct PDU_LLDATA_GPR {
	pdu_lldata_h_s_t    pdu_lldata_h;
	gps_coordinates_s_t pdu_lldata_payload;
} pdu_lldata_gps_s_t;  
//--------------------------------------------------  
//LL TMP Data PDU
typedef struct PDU_LLDATA_TMP {
	pdu_lldata_h_s_t    pdu_lldata_h;
	int32_t	            payload_tmp;
} pdu_lldata_tmp_s_t;  
//--------------------------------------------------      
#pragma pack ()

//Functions' prototypes
void sys_init(void); 
unsigned int get_adv_ch_freq(unsigned short adv_ch_idx); 
unsigned int get_data_ch_freq(unsigned short data_ch_idx);
void wp_set_ch_index(unsigned short ch_idx);
void wp_set_aa(unsigned int aa);
