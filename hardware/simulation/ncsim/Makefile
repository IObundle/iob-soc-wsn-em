ROOT_DIR:=../../..

defmacro:=-define 
incdir:=-incdir 

include ../simulation.mk

#simulator flags
CFLAGS = -errormax 15 -status -update -linedebug -sv $(INCLUDE) $(DEFINE)
EFLAGS = -errormax 15 -access +wc -status
SFLAGS = -errormax 15 -status -CMDFILE $(ROOT_DIR)/hardware/testbench

export VSRC CFLAGS EFLAGS SFLAGS 

#simulate
sim: $(VSRC) $(VHDR) firmware boot.hex demod_coeffs noise_floor
	./run_ncsim.sh
	grep -v xcelium xmsim.log | grep -v xmsim $(LOG)

clean: hw-clean
	@rm -rf xcelium.d xmsim.key *.log *.v $(OUTPUT_FILES)

.PHONY: sim clean
