#DEFINES

BAUD=$(SIM_BAUD)

#ddr controller address width
DEFINE+=$(defmacro)DDR_ADDR_W=24

#vcd dump
ifeq ($(VCD),1)
DEFINE+=$(defmacro)VCD
endif

ifeq ($(DBG),1)
DEFINE+=$(defmacro)DBG
endif


include $(ROOT_DIR)/hardware/hardware.mk

#ASIC libs
ifeq ($(SYNTH),1)
LIBS=/opt/ic_tools/pdk/faraday/umc130/LL/fsc0l_d/2009Q2v3.0/GENERIC_CORE/FrontEnd/verilog/fsc0l_d_generic_core_30.lib
else ifeq ($(PR),1)
LIBS=/opt/ic_tools/pdk/faraday/umc130/LL/fsc0l_d/2009Q2v3.0/GENERIC_CORE/FrontEnd/verilog/fsc0l_d_generic_core_30.lib
endif



#SOURCES
#asic sources
ifeq ($(SYNTH),1)
VSRC=$(ASIC_DIR)/synth/system_synth.v \
$(wildcard $(ASIC_DIR)/memory/bootrom/SP*.v) \
$(wildcard $(ASIC_DIR)/memory/sram/SH*.v)
endif

ifeq ($(PR),1)
VSRC=$(ASIC_DIR)/pr/system_pr.v \
$(wildcard $(ASIC_DIR)/memory/bootrom/SP*.v) \
$(wildcard $(ASIC_DIR)/memory/sram/SH*.v)
endif
#ddr memory
VSRC+=$(AXIMEM_DIR)/rtl/axi_ram.v
#testbench
VSRC+=$(TB_DIR)/system_tb.v \
soc_tb.v \
$(ADPLL_TB_SVSRC)

#simulation output files
OUTPUT_FILES:=*.txt

run: sim self-checker

self-checker:
ifeq ($(DBG),1)
	$(ADPLL_PY_DIR)/self-checker.py -f $(FREQ_CHANNEL) -s soc0
	$(ADPLL_PY_DIR)/self-checker.py -f $(FREQ_CHANNEL)-1 -s soc1
else
	@echo "Warning: DBG flag is disabled! To run self-checker, please re-run simulation with DBG flag enabled."
endif

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
