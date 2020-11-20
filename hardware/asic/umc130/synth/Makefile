ROOT_DIR:=../../../..

include ../../asic.mk

RTL_COMPILER:=source /opt/ic_tools/init/init-rc14_25_hf000


run: rom ram $(VHDR) $(VSRC)
	echo "set INCLUDE [list $(INCLUDE)]" > inc.tcl
	echo "set DEFINE [list $(DEFINE)]" > defs.tcl
	echo "set VHDR [glob $(VHDR)]" > vhdr.tcl
	echo "set VSRC [glob $(VSRC)]" > vsrc.tcl
	cp $(ROM_DIR)/*$(CASE).lib $(RAM_DIR)/*$(CASE).lib .
	$(RTL_COMPILER) && echo "quit" | rc -files inc.tcl -files defs.tcl -files vhdr.tcl -files vsrc.tcl -files synscript.tcl
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
	@find *.tcl -type f -not -name synscript.tcl -not -name powscript.tcl -delete

.PHONY: run rom ram gen_mem clean
