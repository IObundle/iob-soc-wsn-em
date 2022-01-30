ROOT_DIR:=../../../..

defmacro:=-define 
incdir:=-incdir 

include ../../asic.mk

#vcd dump
ifeq ($(VCD),1)
DEFINE+=$(defmacro)VCD
endif

ifeq ($(DBG),1)
DEFINE+=$(defmacro)DBG
endif

ifeq ($(SYNTH),1)
SYSTEM_FILE=$(SYNTH_DIR)/system_synth.v
endif

ifeq ($(PR),1)
SYSTEM_FILE=$(PR_DIR)/system_pr.v
endif

#SOURCES
VSRC+=system_asic.v \
$(wildcard $(ROM_DIR)/SP*.v) \
$(wildcard $(RAM_DIR)/SH*.v)
#ddr memory
VSRC+=$(AXIMEM_DIR)/rtl/axi_ram.v
#testbench
VSRC+=system_tb.v \
soc_asic_tb.v \
soc_tb.v \
$(ADPLL_TB_SVSRC)

run: sim self-checker

self-checker:
ifeq ($(DBG),1)
	$(ADPLL_PY_DIR)/self-checker.py -f $(FREQ_CHANNEL) -s soc0
	$(ADPLL_PY_DIR)/self-checker.py -f $(FREQ_CHANNEL)-1 -s soc1
else
	@echo "Warning: DBG flag is disabled! To run self-checker, please re-run simulation with DBG flag enabled."
endif

#RULES
system_asic.v: $(SYSTEM_FILE)
	cp $< $@
	sed -i 's/ picorv32_pcpi_fast_mul(/ picorv32_pcpi_fast_mul_asic(/g;s/ picorv32_pcpi_fast_mul / picorv32_pcpi_fast_mul_asic /g' $@
	sed -i 's/ picorv32_pcpi_div(/ picorv32_pcpi_div_asic(/g;s/ picorv32_pcpi_div / picorv32_pcpi_div_asic /g' $@
	sed -i 's/ iob_picorv32(/ iob_picorv32_asic(/g;s/ iob_picorv32 / iob_picorv32_asic /g' $@
	sed -i 's/ boot_ctr(/ boot_ctr_asic(/g;s/ boot_ctr / boot_ctr_asic /g' $@
	sed -i 's/ sram(/ sram_asic(/g;s/ sram / sram_asic /g' $@
	sed -i 's/ int_mem(/ int_mem_asic(/g;s/ int_mem / int_mem_asic /g' $@
	sed -i 's/ iref(/ iref_asic(/g;s/ iref / iref_asic /' $@
	sed -i 's/ limiter(/ limiter_asic(/g;s/ limiter / limiter_asic /g' $@
	sed -i 's/ lpf(/ lpf_asic(/g;s/ lpf / lpf_asic /g' $@
	sed -i 's/ mixer(/ mixer_asic(/g;s/ mixer / mixer_asic /g' $@
	sed -i 's/ pa(/ pa_asic(/g;s/ pa / pa_asic /g' $@
	sed -i 's/ timer_core(/ timer_core_asic(/g;s/ timer_core / timer_core_asic /g' $@
	sed -i 's/ iob_timer(/ iob_timer_asic(/g;s/ iob_timer / iob_timer_asic /g' $@
	sed -i 's/ fsk_demod(/ fsk_demod_asic(/g;s/ fsk_demod / fsk_demod_asic /g' $@
	sed -i 's/ aa_find(/ aa_find_asic(/g;s/ aa_find / aa_find_asic /g' $@
	sed -i 's/ byte_s2p(/ byte_s2p_asic(/g;s/ byte_s2p / byte_s2p_asic /g' $@
	sed -i 's/ rx(/ rx_asic(/g;s/ rx / rx_asic /g' $@
	sed -i 's/ byte_p2s(/ byte_p2s_asic(/g;s/ byte_p2s / byte_p2s_asic /g' $@
	sed -i 's/ crc(/ crc_asic(/g;s/ crc / crc_asic /g' $@
	sed -i 's/ syn_fifo(/ syn_fifo_asic(/g;s/ syn_fifo / syn_fifo_asic /g' $@
	sed -i 's/ whitening(/ whitening_asic(/g;s/ whitening / whitening_asic /g' $@
	sed -i 's/ tx(/ tx_asic(/g;s/ tx / tx_asic /g' $@
	sed -i 's/ txrx(/ txrx_asic(/g;s/ txrx / txrx_asic /g' $@
	sed -i 's/ iob_uart(/ iob_uart_asic(/g;s/ iob_uart / iob_uart_asic /g' $@
	sed -i 's/ system(/ system_asic(/g;s/ system / system_asic /g' $@

#create testbench
system_tb.v: $(TB_DIR)/system_tb.v
	cp $< $@
	sed -i '0,/soc_tb #(/{s/soc_tb #(/soc_asic_tb/}' $@
	sed -i -e '/soc_asic_tb/,+2d' $@
	sed -i 's/soc0 (/soc_asic_tb soc0 (/g' $@

soc_asic_tb.v: soc_tb.v
	cp $< $@
	sed -i 's/soc_tb/soc_asic_tb/g' $@
	sed -i 's/system #(/system_asic/g' $@
	sed -i -e '/system_asic/,+2d' $@
	sed -i 's/uut (/system_asic uut (/g' $@

soc_tb.v:
	cp $(TB_DIR)/soc_core_tb.v $@  # create soc_tb.v
	$(foreach p, $(PERIPHERALS), if [ `ls -1 $(SUBMODULES_DIR)/$p/hardware/include/*.vh 2>/dev/null | wc -l ` -gt 0 ]; then $(foreach f, $(shell echo `ls $(SUBMODULES_DIR)/$p/hardware/include/*.vh`), sed -i '/PHEADER/a `include \"$f\"' $@;) break; fi;) # insert header files
	$(foreach p, $(PERIPHERALS), if test -f $(SUBMODULES_DIR)/$p/hardware/include/pio.v; then sed s/input/wire/ $(SUBMODULES_DIR)/$p/hardware/include/pio.v | sed s/output/wire/  | sed s/\,/\;/ > wires_tb.v; sed -i '/PWIRES/r wires_tb.v' $@; fi;) # declare and insert wire declarations
	$(foreach p, $(PERIPHERALS), if test -f $(SUBMODULES_DIR)/$p/hardware/include/pio.v; then sed s/input// $(SUBMODULES_DIR)/$p/hardware/include/pio.v | sed s/output// | sed 's/\[.*\]//' | sed 's/\([A-Za-z].*\),/\.\1(\1),/' > ./ports.v; sed -i '/PORTS/r ports.v' $@; fi;) #insert and connect pins in uut instance
	$(foreach p, $(PERIPHERALS), if test -f $(SUBMODULES_DIR)/$p/hardware/include/inst_tb.sv; then sed -i '/endmodule/e cat $(SUBMODULES_DIR)/$p/hardware/include/inst_tb.sv' $@; fi;) # insert peripheral instances

VSRC+=$(foreach p, $(PERIPHERALS), $(shell if test -f $(SUBMODULES_DIR)/$p/hardware/testbench/module_tb.sv; then echo $(SUBMODULES_DIR)/$p/hardware/testbench/module_tb.sv; fi;)) #add test cores to list of sources

#ASIC libs
LIBS=/opt/ic_tools/pdk/faraday/umc130/LL/fsc0l_d/2009Q2v3.0/GENERIC_CORE/FrontEnd/verilog/fsc0l_d_generic_core_30.lib

#simulator flags
CFLAGS = -errormax 15 -status -update -linedebug -sv $(LIBS) $(INCLUDE) $(DEFINE)
EFLAGS = -errormax 15 -access +wc -status
SFLAGS = -errormax 15 -status -CMDFILE $(ROOT_DIR)/hardware/testbench

export VSRC CFLAGS EFLAGS SFLAGS 

#simulate
sim: $(VSRC) $(VHDR) firmware boot.hex demod_coeffs noise_floor
	./run_ncsim.sh
	grep -v xcelium xmsim.log | grep -v xmsim $(LOG)

clean: hw-clean
	@rm -rf xcelium.d xmsim.key *.log *.v *.txt


.PRECIOUS: system.vcd

.PHONY: run sim self-checker clean
