#include "interconnect.h"
#include "pa.h"

// Memory map
#include "pa_mem_map.h"

//base address
int pa;

void pa_init(int base_address) {
  // capture base address
  pa = base_address;
}

void pa_set_pd(char value) {
  IO_SET(pa, PA_PD, (int)value);
}

void pa_set_mode(char value) {
  IO_SET(pa, PA_MODE, (int)value);
}
