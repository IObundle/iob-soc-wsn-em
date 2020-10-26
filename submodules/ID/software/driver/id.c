#include "interconnect.h"
#include "id.h"

// Memory map
#include "id_mem_map.h"

//base address
int id;

void id_init(int base_address) {
  // capture base address
  id = base_address;
}

int id(void) {
  return (IO_GET(id, ID_VALUE));
}
