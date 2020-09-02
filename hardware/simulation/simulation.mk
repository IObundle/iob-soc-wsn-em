include $(ROOT_DIR)/hardware/hardware.mk

#testbench defmacros
ifeq ($(VCD),1)
DEFINE+=$(defmacro)VCD
endif

#testbench source files
VSRC+=system_tb.v $(AXI_MEM_DIR)/rtl/axi_ram.v

#create testbench system_tb.v
system_tb.v:
	cp $(TB_DIR)/system_core_tb.v $@  # create system_tb.v
	$(foreach p, $(PERIPHERALS), if [ `ls -1 $(SUBMODULES_DIR)/$p/hardware/include/*.vh 2>/dev/null | wc -l ` -gt 0 ]; then $(foreach f, $(shell echo `ls $(SUBMODULES_DIR)/$p/hardware/include/*.vh`), sed -i '/PHEADER/a `include \"$f\"' $@;) break; fi;) # insert header files
	rm  wires_tb.v; touch wires_tb.v
	$(foreach p, $(PERIPHERALS), sed s/input/wire/ $(SUBMODULES_DIR)/$p/hardware/include/pio.v | sed s/output/wire/  | sed s/\,/\;/ >> wires_tb.v;) #declare wires
	sed -i '/PWIRES/e cat wires_tb.v' $@; #insert wire declarations
	$(foreach p, $(PERIPHERALS), sed -i '/endmodule/e cat $(SUBMODULES_DIR)/$p/hardware/include/inst_tb.v' $@;) # insert peripheral instances

VSRC+=$(foreach p, $(PERIPHERALS), $(shell if test -f $(SUBMODULES_DIR)/$p/hardware/testbench/module_tb.sv; then echo $(SUBMODULES_DIR)/$p/hardware/testbench/module_tb.sv; fi;)) #add test cores to list of sources
