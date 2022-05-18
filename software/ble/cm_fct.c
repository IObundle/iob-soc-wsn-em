#include "iob-uart.h"
#include "iob_timer.h"
#include "txrx.h"
#include "ble.h"
#include "cm_def.h"
#include "tmp.h"


//System Init
void sys_init(void){
    ble_init(); 
    uart_init(UART_BASE,FREQ/BAUD); 
    //timer_init(TIMER_BASE);
    tmp_init(TMP_BASE);		      
}

//Get the advertising channel frequency in MHz - Advertising channels: 37, 38, 39
unsigned int get_adv_ch_freq(unsigned short adv_ch_idx) {   
   unsigned int freq;    
   if (adv_ch_idx == 37) {
   	freq = 2402;		
   } else if (adv_ch_idx == 38) {
   	freq = 2426;	
   } else if (adv_ch_idx == 39) {	
   	freq = 2480;
   }	
   return freq;  
}

//Get the data channel frequency in MHz
unsigned int get_data_ch_freq(unsigned short data_ch_idx) {   
   unsigned int freq;       
   if(data_ch_idx < 11){
      freq = 2402 + 2*data_ch_idx + 2;
   } else {
      freq = 2402 + 2*data_ch_idx + 4;
   }
   return freq;  
}

//Generate a data channel map
uint64_t gen_dataChMap(uint8_t dataChIdx[], unsigned int size){
    uint64_t dataChM=0;
    for(int i=0; i<size; i++){
	 dataChM = dataChM | ((uint64_t)1 << (39 - dataChIdx[i]));            
    } 
    return dataChM;  
}

//Set the whitener/dewhitener channel index
void wp_set_ch_index(unsigned short ch_idx) {
     txrx_set_ch_idx(ch_idx);
}

//Set the access address
void wp_set_aa(unsigned int aa) {
     txrx_set_aa(aa);
}

void print_data(uint64_t data_in, uint32_t size){
     int buffer[size];      
     for (int i=0; i<size; i++) {buffer[i]=0;}
     uart_printf("0x"); for (int i=size-1; i>=0; i--){
        buffer[i]=N_SHIFT(data_in, i);
	if(!buffer[i]){uart_printf("00");}
	else if(buffer[i]<16){uart_printf("0%x", buffer[i]);}
	else{uart_printf("%x", buffer[i]);}
     } 
}
