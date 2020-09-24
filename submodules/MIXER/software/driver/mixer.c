#include "interconnect.h"
#include "mixer.h"

// Memory map
#include "mixer_mem_map.h"

//base address
int mixer;

void mixer_init(int base_address) {
  // capture base address
  mixer = base_address;
}

void mixer_set_pd(char value) {
  IO_SET(mixer, MIXER_PD, (int)value);
}

void mixer_set_ota(char value) {
  IO_SET(mixer, MIXER_OTA, (int)value);
}

void mixer_set_buff(char value) {
  IO_SET(mixer, MIXER_BUFF, (int)value);
}
