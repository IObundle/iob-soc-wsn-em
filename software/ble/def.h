#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#ifdef DBUG
#include "printf.h"
#endif

#define SN_ID 0
#define BS_ID 1
#define ITER  12

#define MAX_N_SN 6
#define MAX_N_BYTES 39
#define MAX_N_DATA_CHANNELS 37
#define ADV_CH_FIRST 37

#define ADV_IND_H_LEN 2
#define LL_DATA_H_LEN 2
#define LL_DATA_GPS_P_LEN 18
#define LL_DATA_TMP_P_LEN 4
#define LL_CONTROL_P_LEN 4  
#define CRC_LEN 3

//SN TX - Configuration of a connectable undirected advertising packet's header
//TxAdd: random device address
//RFU_2 temporarily set for debugging purpose 			  
#define ADV_IND 0
#define ADV_IND_H_RFU_1 0
#define ADV_IND_H_H_TxAdd 1    			
#define ADV_IND_H_RxAdd 0
#define ADV_IND_P_LEN 8
#define ADV_IND_H_RFU_2 1 

#define ADV_DIRECT_IND 1

//BS TX - Configuration of a connection request packet's header  
//TxAdd, RxAdd, Length and RFU_2 temporarily set for debugging purpose 
#define CONNECT_REQ 5			
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
	
typedef enum SN_MODES {
	MODE_SN_STANDBY=1,
	MODE_SN_TX_ADV_IND,	
	MODE_SN_TX_ADV_DIRECT_IND,	
	MODE_SN_RX_CONNECT_REQ,
	MODE_SN_TX_DATA_GPS,
	MODE_SN_RX_ACK_GPS, 
	MODE_SN_TX_DATA_TMP,
	MODE_SN_RX_END_CONNECTION
} sn_modes_e_t;

typedef enum BS_MODES {
	MODE_BS_STANDBY=11,
	MODE_BS_RX_ADV_IND, 	
	MODE_BS_RX_ADV_DIRECT_IND,
	MODE_BS_TX_CONNECT_REQ,	
	MODE_BS_RX_DATA_GPS,
	MODE_BS_TX_ACK_GPS,
	MODE_BS_RX_DATA_TMP,
	MODE_BS_TX_END_CONNECTION		
} bs_modes_e_t;

typedef enum AA_STATUS {
	AA_FREE=1,
	GPS_WAIT,
	AA_ATTR
} aa_status_e_t;

typedef struct SN {    	  
    	uint32_t  aa;     
    	uint32_t  nextState; 
    	uint8_t   id;   
    	uint8_t   adv;  
	char      buffer_tx[MAX_N_BYTES];
	char	  buffer_rx[MAX_N_BYTES];
} sn_s_t;
  
typedef struct BS {
    	uint32_t aa;
    	uint32_t nextState;
    	uint32_t isBusy;
    	uint8_t  id;
	char     buffer_tx[MAX_N_BYTES];
	char	 buffer_rx[MAX_N_BYTES];	
} bs_s_t;

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
//Connectable undirected advertising PDU's payload
typedef struct ADV_IND_PAYLOAD {
	uint64_t AdvA    : 48,	  
		 AdvData : 16;	      
} adv_ind_payload_s_t;	    
 
// Connectable undirected advertising PDU
typedef struct ADV_IND_PDU {
	pdu_adv_h_s_t        pdu_adv_ind_h;
	adv_ind_payload_s_t  pdu_adv_ind_payload; 
} adv_ind_pdu_s_t;
//-------------------------------------------------- 
//Connectable directed advertising PDU's payload
typedef struct ADV_DIRECT_IND_PAYLOAD {
	uint64_t AdvA  : 64,   //48    
		 InitA : 64;   //48	   
} adv_direct_ind_payload_s_t;
//-------------------------------------------------- 
typedef struct CONNECT_REQ_LLDATA {
	uint64_t  AA        : 32,	//4 bytes
		  CRCInit   : 24,	//3 bytes
		  WinSize   : 8,      	//1 byte 
		  WinOffset : 16,     	//2 bytes
		  Interval  : 16,     	//2 bytes
		  Latency   : 16,     	//2 bytes
		  Timeout   : 16,     	//2 bytes
		  ChM	    : 40,       //5 bytes
		  Hop	    : 5,        //5 bits 
	          SCA	    : 3;        //3 bits 
} connect_req_lldata_s_t;

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
	//connect_req_lldata_s_t LLData;	
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

typedef struct {
     adv_ind_pdu_s_t sn_tx_adv_ind_pdu;    //Connectable undirected advertising PDU
     uint32_t        nextState;
     uint32_t        sn_ch_freq;  
     uint32_t        pdu_size_1;	   //for debugging purpose   
}sn_tx_adv_param_s_t;

typedef struct {
     connect_request_pdu_s_t sn_rx_connect_request_pdu;    //Connection request PDU
     uint32_t     nextState;  
     uint32_t     pdu_size_1;	//for debugging purpose
     uint32_t     result;	//for debugging purpose
     uint16_t     sn_adv_ch_idx;
     uint8_t      sn_adv;
     int8_t 	  nbytes;	//for debugging purpose    
} sn_rx_cnt_req_param_s_t;

typedef struct {
     pdu_lldata_gps_s_t sn_tx_data_gps_pdu;    //GPS Coordinates data PDU
     uint32_t     nextState;  
     uint32_t     sn_ch_freq;
     uint32_t     pdu_size_1;	    //for debugging purpose
     uint16_t     sn_data_ch_idx;   //for debugging purpose
     uint8_t      data_ch[MAX_N_DATA_CHANNELS];
} sn_tx_gps_param_s_t;
    
typedef struct {
     pdu_llcontrol_s_t  sn_rx_pdu_llcontrol;    //ACK or TERMINATE PDU     
     uint32_t           nextState;  
     uint32_t           pdu_size_1;	    //for debugging purpose 
     uint32_t           result;	    //for debugging purpose
     int8_t 	        nbytes;	    //for debugging purpose            
} sn_rx_llcontrol_param_s_t;

typedef struct {
     pdu_lldata_tmp_s_t sn_tx_lldata_tmp_pdu;     //TMP data PDU
     uint32_t     nextState;  
     uint32_t     pdu_size_1;	    //for debugging purpose 
} sn_tx_tmp_param_s_t;

unsigned int get_adv_ch_freq(unsigned short adv_ch_idx);
unsigned int get_data_ch_freq(unsigned short data_ch_idx);
void wp_set_ch_index(unsigned short ch_idx);
void wp_set_aa(unsigned int aa);
