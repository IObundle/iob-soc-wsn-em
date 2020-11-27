ID_SW_DIR:=$(ID_DIR)/software/driver
ID_INC_DIR:=$(ID_DIR)/hardware/include

#include
INCLUDE+=-I$(ID_SW_DIR)

#headers
HDR+=$(ID_SW_DIR)/*.h \
id_mem_map.h

#sources
SRC+=$(ID_SW_DIR)/*.c

id_mem_map.h:
	sed -n 's/`ID_ADDR_W//p' $(ID_INC_DIR)/id.vh | sed 's/`/#/g' | sed "s/('d//g" | sed 's/)//g' > ./$@
