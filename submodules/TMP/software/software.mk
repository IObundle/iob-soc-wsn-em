TMP_SW_DIR:=$(TMP_DIR)/software/driver
TMP_INC_DIR:=$(TMP_DIR)/hardware/include

#include
INCLUDE+=-I$(TMP_SW_DIR)

#headers
HDR+=$(TMP_SW_DIR)/*.h \
tmp_mem_map.h

#sources
SRC+=$(TMP_SW_DIR)/*.c

tmp_mem_map.h:
	sed -n 's/`TMP_ADDR_W//p' $(TMP_INC_DIR)/tmp.vh | sed 's/`/#/g' | sed "s/('d//g" | sed 's/)//g' > ./$@
