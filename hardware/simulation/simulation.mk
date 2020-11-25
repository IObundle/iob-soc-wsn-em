#DEFINES

BAUD=$(SIM_BAUD)

#ddr controller address width
DEFINE+=$(defmacro)DDR_ADDR_W=24

#vcd dump
ifeq ($(VCD),1)
DEFINE+=$(defmacro)VCD
endif


include $(ROOT_DIR)/hardware/hardware.mk




#SOURCES
#ddr memory
VSRC+=$(CACHE_DIR)/submodules/AXIMEM/rtl/axi_ram.v
#testbench
VSRC+=$(TB_DIR)/system_tb.v \
soc_tb.v \
$(ADPLL_TB_SVSRC)

#simulation output files
OUTPUT_FILES:=*.txt

run: sim self-checker

self-checker:
	python3 $(ADPLL_PY_DIR)/self-checker.py 0 $(FREQ_CHANNEL) SoC0
	python3 $(ADPLL_PY_DIR)/self-checker.py 0 $(FREQ_CHANNEL)-1 SoC1

#RULES
#create testbench
soc_tb.v:
	cp $(TB_DIR)/soc_core_tb.v $@  # create soc_tb.v
	$(foreach p, $(PERIPHERALS), if [ `ls -1 $(SUBMODULES_DIR)/$p/hardware/include/*.vh 2>/dev/null | wc -l ` -gt 0 ]; then $(foreach f, $(shell echo `ls $(SUBMODULES_DIR)/$p/hardware/include/*.vh`), sed -i '/PHEADER/a `include \"$f\"' $@;) break; fi;) # insert header files
	$(foreach p, $(PERIPHERALS), if test -f $(SUBMODULES_DIR)/$p/hardware/include/pio.v; then sed s/input/wire/ $(SUBMODULES_DIR)/$p/hardware/include/pio.v | sed s/output/wire/  | sed s/\,/\;/ > wires_tb.v; sed -i '/PWIRES/r wires_tb.v' $@; fi;) # declare and insert wire declarations
	$(foreach p, $(PERIPHERALS), if test -f $(SUBMODULES_DIR)/$p/hardware/include/pio.v; then sed s/input// $(SUBMODULES_DIR)/$p/hardware/include/pio.v | sed s/output// | sed 's/\[.*\]//' | sed 's/\([A-Za-z].*\),/\.\1(\1),/' > ./ports.v; sed -i '/PORTS/r ports.v' $@; fi;) #insert and connect pins in uut instance
	$(foreach p, $(PERIPHERALS), if test -f $(SUBMODULES_DIR)/$p/hardware/include/inst_tb.sv; then sed -i '/endmodule/e cat $(SUBMODULES_DIR)/$p/hardware/include/inst_tb.sv' $@; fi;) # insert peripheral instances

VSRC+=$(foreach p, $(PERIPHERALS), $(shell if test -f $(SUBMODULES_DIR)/$p/hardware/testbench/module_tb.sv; then echo $(SUBMODULES_DIR)/$p/hardware/testbench/module_tb.sv; fi;)) #add test cores to list of sources


.PRECIOUS: system.vcd

.PHONY: run self-checker
