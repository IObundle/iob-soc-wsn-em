TXRX_SW_DIR:=$(TXRX_DIR)/software/driver
TXRX_INC_DIR:=$(TXRX_DIR)/hardware/include

#include
INCLUDE+=-I$(TXRX_SW_DIR)

#headers
HDR+=$(TXRX_SW_DIR)/*.h \
txrx_mem_map.h

#sources
SRC+=$(TXRX_SW_DIR)/*.c

txrx_mem_map.h:
	sed -n 's/`TXRX_ADDR_W//p' $(TXRX_INC_DIR)/header.vh | sed 's/`/#/g' | sed "s/('d//g" | sed 's/)//g' > ./$@
