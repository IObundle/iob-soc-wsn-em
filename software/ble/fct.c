// Get the advertising channel frequency in MHz - Advertising channels: 37, 38, 39
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

// Get the data channel frequency in MHz - Data channels: 5, 22, 34
unsigned int get_data_ch_freq(unsigned short data_ch_idx) {   
   unsigned int freq;    
   if (data_ch_idx == 5) {
   	freq = 2412;		
   } else if (data_ch_idx == 22) {
   	freq = 2446;	
   } else if (data_ch_idx == 34) {	
   	freq = 2470;
   }	
   return freq;  
}
