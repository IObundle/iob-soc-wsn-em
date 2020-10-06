IREF_SW_DIR:=$(IREF_DIR)/software/driver
IREF_INC_DIR:=$(IREF_DIR)/hardware/include

#include
INCLUDE+=-I$(IREF_SW_DIR)

#headers
HDR+=$(IREF_SW_DIR)/*.h \
iref_mem_map.h

#sources
SRC+=$(IREF_SW_DIR)/*.c

iref_mem_map.h:
	sed -n 's/`IREF_ADDR_W//p' $(IREF_INC_DIR)/header.vh | sed 's/`/#/g' | sed "s/('d//g" | sed 's/)//g' > ./$@
