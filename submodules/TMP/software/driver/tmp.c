#include <stdint.h>

#include "interconnect.h"
#include "tmp.h"

#include "tmp_mem_map.h"   //Memory map

static int tmp;   //base address

void tmp_init(int base_address){
   tmp = base_address;
}

int32_t get_tmp(void){
   return(IO_GET(tmp, TMP_VALUE));
} 
