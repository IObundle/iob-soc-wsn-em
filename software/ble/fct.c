// Get the advertising channel frequency in MHz
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
