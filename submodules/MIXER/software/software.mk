MIXER_SW_DIR:=$(MIXER_DIR)/software/driver
MIXER_INC_DIR:=$(MIXER_DIR)/hardware/include

#include
INCLUDE+=-I$(MIXER_SW_DIR)

#headers
HDR+=$(MIXER_SW_DIR)/*.h \
mixer_mem_map.h

#sources
SRC+=$(MIXER_SW_DIR)/*.c

mixer_mem_map.h:
	sed -n 's/`MIXER_ADDR_W//p' $(MIXER_INC_DIR)/mixer.vh | sed 's/`/#/g' | sed "s/('d//g" | sed 's/)//g' > ./$@
