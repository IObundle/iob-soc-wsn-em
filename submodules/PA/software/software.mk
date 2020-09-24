PA_SW_DIR:=$(PA_DIR)/software/driver
PA_INC_DIR:=$(PA_DIR)/hardware/include

#include
INCLUDE+=-I$(PA_SW_DIR)

#headers
HDR+=$(PA_SW_DIR)/*.h \
pa_mem_map.h

#sources
SRC+=$(PA_SW_DIR)/*.c

pa_mem_map.h:
	sed -n 's/`PA_ADDR_W//p' $(PA_INC_DIR)/header.vh | sed 's/`/#/g' | sed "s/('d//g" | sed 's/)//g' > ./$@
