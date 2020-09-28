#include "interconnect.h"
#include "iref.h"

// Memory map
#include "iref_mem_map.h"

//base address
int iref;

void iref_init(int base_address) {
  // capture base address
  iref = base_address;
}

void iref_set_pd(char value) {
  IO_SET(iref, IREF_PD, (int)value);
}

void iref_set_fc(char value) {
  IO_SET(iref, IREF_FC, (int)value);
}
