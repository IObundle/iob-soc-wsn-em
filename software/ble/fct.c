unsigned int get_size (unsigned char adv_or_data, unsigned char field_num) {
   unsigned int size=0;
   if (adv_or_data == 1) { //ADV_IND
	if (field_num == 1) { //AdvA in payload
	    size = 6;
	} else if (field_num == 2) { //AdvData in paylaod
	    size = 1;
	} 
   }	
   return size;	
}
