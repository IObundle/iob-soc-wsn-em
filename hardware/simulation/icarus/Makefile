ROOT_DIR:=../../..

defmacro:=-D
incdir:=-I

include ../simulation.mk

#simulator flags
VLOG = iverilog -W all -g2005-sv $(INCLUDE) $(DEFINE) 

#run the simulator
sim: $(VSRC) $(VHDR) firmware boot.hex demod_coeffs noise_floor
	$(VLOG) $(VSRC)
	./a.out $(LOG)

clean: hw-clean
	@rm -f ./a.out *.log *.v $(OUTPUT_FILES)

.PHONY: sim clean
