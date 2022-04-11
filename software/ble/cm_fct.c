#include "iob-uart.h"
#include "iob_timer.h"
#include "txrx.h"
#include "ble.h"
#include "cm_def.h"


//System Init
void sys_init(void){
    ble_init(); 
    uart_init(UART_BASE,FREQ/BAUD); 
    timer_init(TIMER_BASE);		      
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

//Get the data channel frequency in MHz - Data channels: 5, 22, 34
unsigned int get_data_ch_freq(unsigned short data_ch_idx) {   
   unsigned int freq;    
   
   if (data_ch_idx == 5) {
   	freq = 2414;		
   } else if (data_ch_idx == 22) {
   	freq = 2450;	
   } else if (data_ch_idx == 34) {	
   	freq = 2474;
   }	
   return freq;  
}

//Set the whitener/dewhitener channel index
void wp_set_ch_index(unsigned short ch_idx) {
     txrx_set_ch_idx(ch_idx);
}

//Set the access address
void wp_set_aa(unsigned int aa) {
     txrx_set_aa(aa);
}


