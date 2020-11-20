ROOT_DIR:=../../../..

include ../../asic.mk

RTL_COMPILER:=source /opt/ic_tools/init/init-rc14_25_hf000


run: rom ram
	echo "set SRCH_PATH [list $()]" > spath.tcl
	echo "set DEFINE [list $(DEFINE)]" > defs.tcl
	echo "set VSRC [list $(VSRC)]" > vsrc.tcl
	echo "set INCLUDE [list $(VHDR)]" > inc.tcl
	cp $(ROM_DIR)/*$(CASE).lib $(RAM_DIR)/*$(CASE).lib .
	$(RTL_COMPILER) $(DISPLAY_EXP) && echo "quit" | rc -files spath.tcl -files defs.tcl -files vsrc.tcl -files inc.tcl -files synscript.tcl
	echo "quit"

rom: gen_mem
	$(PYTHON_DIR)/memwrapper_make.py fsc0l_d sp $(MEM_SIZE_ROM) 32 1 > $(MEMW_DIR)/rom.v

ram: gen_mem
	$(PYTHON_DIR)/memwrapper_make.py fsc0l_d sh $(MEM_SIZE_RAM) 8 4 1 > $(MEMW_DIR)/ram.v

gen_mem:
	make -C ../memory
	cp $(SW_DIR)/system.h .
	rm -fR $(MEMW_DIR)
	mkdir $(MEMW_DIR)

clean:
	make -C ../memory clean
	@rm -rf $(MEMW_DIR) *~ *.txt rc.cmd* rc.* fv libscore_work *.sdc *.v *.h *.lib

.PHONY: run rom ram gen_mem clean
