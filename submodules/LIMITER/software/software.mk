LIMITER_SW_DIR:=$(LIMITER_DIR)/software/driver
LIMITER_INC_DIR:=$(LIMITER_DIR)/hardware/include

#include
INCLUDE+=-I$(LIMITER_SW_DIR)

#headers
HDR+=$(LIMITER_SW_DIR)/*.h \
limiter_mem_map.h

#sources
SRC+=$(LIMITER_SW_DIR)/*.c

limiter_mem_map.h:
	sed -n 's/`LIMITER_ADDR_W//p' $(LIMITER_INC_DIR)/limiter.vh | sed 's/`/#/g' | sed "s/('d//g" | sed 's/)//g' > ./$@
