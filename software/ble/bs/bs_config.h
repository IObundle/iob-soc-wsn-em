#pragma once

#include "cm_def.h"

#ifdef CST
const uint64_t BS_DEVICE_ADDR=0x2B53E85C74A8;
const uint64_t SN_DEVICE_ADDR_OP=0x8C87BAE5D45B;
//Temporary setting for debuggin purpose
static uint64_t bs_whitelist[MAX_N_SN] = {SN_DEVICE_ADDR_OP, 0xF2AFFCFF9B, 0xF3FEFF7F1C, 0xF4FF4FBF7A, 0xF5FFFFFFB2, 0xF6CFAFFF1B, 0xF5FB6FFF5A, 0xF6FDFF1FE, 0xF5FF8FFE2D, 0xF6FF1FBF7C};
#endif
