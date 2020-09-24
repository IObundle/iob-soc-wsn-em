#include "interconnect.h"
#include "lpf.h"

// Memory map
#include "lpf_mem_map.h"

//base address
int lpf;

void lpf_init(int base_address) {
  // capture base address
  lpf = base_address;
}

void lpf_set_pd(char value) {
  IO_SET(lpf, LPF_PD, (int)value);
}

void lpf_set_fc(char value) {
  IO_SET(lpf, LPF_FC, (int)value);
}
