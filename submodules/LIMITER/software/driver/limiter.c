#include "interconnect.h"
#include "limiter.h"

// Memory map
#include "limiter_mem_map.h"

//base address
int limiter;

void limiter_init(int base_address) {
  // capture base address
  limiter = base_address;
}

void limiter_set_pd(char value) {
  IO_SET(limiter, LIMITER_PD, (int)value);
}
