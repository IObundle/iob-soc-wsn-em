ROOT_DIR:=.

#default target
all: system.mk
	make -C hardware/simulation/icarus run BAUD=10000000

#system configuration file
system.mk: system_config.mk
	cp system_config.mk system.mk
	@echo system.mk file created

include ./system.mk

#
# PC
#

pc-emul: system.mk
	make -C software/pc-emul run

pc-clean:
	make -C software/pc-emul clean


#
# SIMULATE
#

sim: sim-clean
	make -C $(FIRM_DIR) run BAUD=$(SIM_BAUD) MODE=$(MODE)
	make -C $(BOOT_DIR) run BAUD=$(SIM_BAUD)
	make -C $(SUBMODULES_DIR)/FSK_DEMOD demod_coeffs
	make -C $(SUBMODULES_DIR)/FSK_DEMOD noise_floor.txt
ifeq ($(SIMULATOR),$(filter $(SIMULATOR), $(LOCAL_SIM_LIST)))
	bash -c "trap 'make sim-display-output' INT TERM KILL; make -C $(SIM_DIR) run BAUD=$(SIM_BAUD)"
else
	ssh $(SIM_USER)@$(SIM_SERVER) "if [ ! -d $(REMOTE_ROOT_DIR) ]; then mkdir -p $(REMOTE_ROOT_DIR); fi"
	rsync -avz --exclude .git $(ROOT_DIR) $(SIM_USER)@$(SIM_SERVER):$(REMOTE_ROOT_DIR)
	bash -c "trap 'make kill-remote-sim' INT TERM KILL; ssh $(SIM_USER)@$(SIM_SERVER) 'cd $(REMOTE_ROOT_DIR); make -C $(SIM_DIR) run INIT_MEM=$(INIT_MEM) USE_DDR=$(USE_DDR) RUN_DDR=$(RUN_DDR) TEST_LOG=$(TEST_LOG) VCD=$(VCD) DBG=$(DBG) BAUD=$(SIM_BAUD)'"
	scp $(SIM_USER)@$(SIM_SERVER):$(REMOTE_ROOT_DIR)/$(SIM_DIR)/soc*.txt $(SIM_DIR)
ifeq ($(TEST_LOG),1)
	scp $(SIM_USER)@$(SIM_SERVER):$(REMOTE_ROOT_DIR)/$(SIM_DIR)/test.log $(SIM_DIR)
endif
ifeq ($(VCD),1)
	scp $(SIM_USER)@$(SIM_SERVER):$(REMOTE_ROOT_DIR)/$(SIM_DIR)/*.vcd $(SIM_DIR)
endif
endif
	make sim-display-output

sim-display-output:
	@echo ""
	@echo "#"
	@echo "# Soc 0"
	@echo "#"
	@echo ""
	@cat $(SIM_DIR)/soc0.txt
	@echo ""
	@echo "#"
	@echo "# Soc 1"
	@echo "#"
	@echo ""
	@cat $(SIM_DIR)/soc1.txt

sim-waves: $(SIM_DIR)/../waves.gtkw $(SIM_DIR)/system.vcd
	gtkwave -a $^ &

kill-remote-sim:
	@echo "INFO: Remote simulator will be killed; ignore following errors"
	ssh $(SIM_USER)@$(SIM_SERVER) 'cd $(REMOTE_ROOT_DIR); killall -q -u $(SIM_USER) -9 a.out; killall -q -u $(SIM_USER) -9 xmsim'
	scp $(SIM_USER)@$(SIM_SERVER):$(REMOTE_ROOT_DIR)/$(SIM_DIR)/soc*.txt $(SIM_DIR)
	make sim-display-output

$(SIM_DIR)/../waves.gtkw $(SIM_DIR)/system.vcd:
	make sim INIT_MEM=$(INIT_MEM) USE_DDR=$(USE_DDR) RUN_DDR=$(RUN_DDR) VCD=$(VCD)

sim-clean: sw-clean hex-clean
	make -C $(SIM_DIR) clean 
ifneq ($(SIMULATOR),$(filter $(SIMULATOR), $(LOCAL_SIM_LIST)))
	rsync -avz --exclude .git $(ROOT_DIR) $(SIM_USER)@$(SIM_SERVER):$(REMOTE_ROOT_DIR)
	ssh $(SIM_USER)@$(SIM_SERVER) 'if [ -d $(REMOTE_ROOT_DIR) ]; then cd $(REMOTE_ROOT_DIR); make -C $(SIM_DIR) clean; fi'
endif

#
# FPGA COMPILE 
#

fpga: system.mk
	make -C $(FIRM_DIR) run BAUD=$(HW_BAUD)
	make -C $(BOOT_DIR) run BAUD=$(HW_BAUD)
ifeq ($(BOARD),$(filter $(BOARD), $(LOCAL_FPGA_LIST)))
	make -C $(BOARD_DIR) compile BAUD=$(HW_BAUD)
else
	ssh $(FPGA_USER)@$(FPGA_SERVER) 'if [ ! -d $(REMOTE_ROOT_DIR) ]; then mkdir -p $(REMOTE_ROOT_DIR); fi'
	rsync -avz --exclude .git $(ROOT_DIR) $(FPGA_USER)@$(FPGA_SERVER):$(REMOTE_ROOT_DIR)
	ssh $(FPGA_USER)@$(FPGA_SERVER) 'cd $(REMOTE_ROOT_DIR); make -C $(BOARD_DIR) compile INIT_MEM=$(INIT_MEM) USE_DDR=$(USE_DDR) RUN_DDR=$(RUN_DDR) BAUD=$(HW_BAUD)'
ifneq ($(FPGA_SERVER),localhost)
	scp $(FPGA_USER)@$(FPGA_SERVER):$(REMOTE_ROOT_DIR)/$(BOARD_DIR)/$(FPGA_OBJ) $(BOARD_DIR)
	scp $(FPGA_USER)@$(FPGA_SERVER):$(REMOTE_ROOT_DIR)/$(BOARD_DIR)/$(FPGA_LOG) $(BOARD_DIR)
endif
endif

fpga-clean: sw-clean
ifeq ($(BOARD),$(filter $(BOARD), $(LOCAL_FPGA_LIST)))
	make -C $(BOARD_DIR) clean
else
	rsync -avz --exclude .git $(ROOT_DIR) $(FPGA_USER)@$(FPGA_SERVER):$(REMOTE_ROOT_DIR)
	ssh $(FPGA_USER)@$(FPGA_SERVER) 'if [ -d $(REMOTE_ROOT_DIR) ]; then cd $(REMOTE_ROOT_DIR); make -C $(BOARD_DIR) clean; fi'
endif

fpga-clean-ip: fpga-clean
ifeq ($(BOARD), $(filter $(BOARD), $(LOCAL_FPGA_LIST)))
	make -C $(BOARD_DIR) clean-ip
else
	ssh $(FPGA_USER)@$(FPGA_SERVER) 'cd $(REMOTE_ROOT_DIR); make -C $(BOARD_DIR) clean-ip'
endif

#
# RUN BOARD
#

board-load: system.mk
ifeq ($(BOARD),$(filter $(BOARD), $(LOCAL_BOARD_LIST)))
	make -C $(BOARD_DIR) load
else
	ssh $(BOARD_USER)@$(BOARD_SERVER) 'if [ ! -d $(REMOTE_ROOT_DIR) ]; then mkdir -p $(REMOTE_ROOT_DIR); fi'
	rsync -avz --exclude .git $(ROOT_DIR) $(BOARD_USER)@$(BOARD_SERVER):$(REMOTE_ROOT_DIR) 
	ssh $(BOARD_USER)@$(BOARD_SERVER) 'cd $(REMOTE_ROOT_DIR); make -C $(BOARD_DIR) load'
endif

board-run: firmware
ifeq ($(BOARD),$(filter $(BOARD), $(LOCAL_BOARD_LIST)))
	make -C $(CONSOLE_DIR) run TEST_LOG=$(TEST_LOG) BAUD=$(HW_BAUD)
else
	ssh $(BOARD_USER)@$(BOARD_SERVER) 'if [ ! -d $(REMOTE_ROOT_DIR) ]; then mkdir -p $(REMOTE_ROOT_DIR); fi'
	rsync -avz --exclude .git $(ROOT_DIR) $(BOARD_SERVER):$(REMOTE_ROOT_DIR)
	bash -c "trap 'make kill-remote-console' INT TERM KILL; ssh $(BOARD_USER)@$(BOARD_SERVER) 'cd $(REMOTE_ROOT_DIR); make -C $(CONSOLE_DIR) run INIT_MEM=$(INIT_MEM) TEST_LOG=$(TEST_LOG) BAUD=$(HW_BAUD)  BOARD=$(BOARD)'"
ifneq ($(TEST_LOG),)
	scp $(BOARD_SERVER):$(REMOTE_ROOT_DIR)/$(CONSOLE_DIR)/test.log $(CONSOLE_DIR)/test.log
endif
endif

kill-remote-console:
	@echo "INFO: Remote console will be killed; ignore following errors"
	ssh $(BOARD_USER)@$(BOARD_SERVER) 'cd $(REMOTE_ROOT_DIR); killall -q -u $(BOARD_USER) -9 console'


board_clean: system.mk
ifeq ($(BOARD),$(filter $(BOARD), $(LOCAL_BOARD_LIST)))
	make -C $(BOARD_DIR) clean
else
	rsync -avz --exclude .git $(ROOT_DIR) $(BOARD_SERVER):$(REMOTE_ROOT_DIR)
	ssh $(BOARD_USER)@$(BOARD_SERVER) 'if [ -d $(REMOTE_ROOT_DIR) ]; then cd $(REMOTE_ROOT_DIR); make -C $(BOARD_DIR) clean; fi'
endif

#
# COMPILE SOFTWARE
#

firmware: system.mk
	make -C $(FIRM_DIR) run

firmware-clean: system.mk
	make -C $(FIRM_DIR) clean

bootloader: firmware
	make -C $(BOOT_DIR) run

bootloader-clean: system.mk
	make -C $(BOOT_DIR) clean

console: system.mk
	make -C $(CONSOLE_DIR) run BAUD=$(HW_BAUD)

console-clean: system.mk
	make -C $(CONSOLE_DIR) clean

sw-clean: firmware-clean bootloader-clean console-clean

hex-clean:
	make -C $(SUBMODULES_DIR)/FSK_DEMOD clean

#
# COMPILE DOCUMENTS
#

doc: system.mk
	make -C document/pb pb.pdf
	make -C document/presentation presentation.pdf

doc-clean: system.mk
	make -C document/pb clean
	make -C document/presentation clean

doc-pdfclean: system.mk
	make -C document/pb pdfclean
	make -C document/presentation pdfclean


#
# TEST ON SIMULATORS AND BOARDS
#

test: test-all-simulators test-all-boards

#test on simulators
test-simulator:
	echo "Testing simulator $(SIMULATOR)";echo Testing simulator $(SIMULATOR)>>test.log
	make sim SIMULATOR=$(SIMULATOR) INIT_MEM=1 USE_DDR=0 RUN_DDR=0 TEST_LOG=1
	cat $(SIM_DIR)/test.log >> test.log
	make sim SIMULATOR=$(SIMULATOR) INIT_MEM=0 USE_DDR=0 RUN_DDR=0 TEST_LOG=1
	cat $(SIM_DIR)/test.log >> test.log
	make sim SIMULATOR=$(SIMULATOR) INIT_MEM=1 USE_DDR=1 RUN_DDR=0 TEST_LOG=1
	cat $(SIM_DIR)/test.log >> test.log
	make sim SIMULATOR=$(SIMULATOR) INIT_MEM=1 USE_DDR=1 RUN_DDR=1 TEST_LOG=1
	cat $(SIM_DIR)/test.log >> test.log
	make sim SIMULATOR=$(SIMULATOR) INIT_MEM=0 USE_DDR=1 RUN_DDR=1 TEST_LOG=1
	cat $(SIM_DIR)/test.log >> test.log

test-all-simulators:
	@rm -f test.log
	$(foreach s, $(SIM_LIST), make test-simulator SIMULATOR=$s;)
	diff -q test.log test/test-sim.log
	@echo SIMULATION TEST PASSED FOR $(SIM_LIST)

#test on boards
test-board-config:
	make fpga-clean BOARD=$(BOARD)
	make fpga BOARD=$(BOARD) INIT_MEM=$(INIT_MEM) USE_DDR=$(USE_DDR) RUN_DDR=$(RUN_DDR)
	make board-clean BOARD=$(BOARD)
	make board-load BOARD=$(BOARD)
	make board-run BOARD=$(BOARD) INIT_MEM=$(INIT_MEM) TEST_LOG=$(TEST_LOG)
ifneq ($(TEST_LOG),)
	cat $(CONSOLE_DIR)/test.log >> test.log
endif

test-board:
	echo "Testing board $(BOARD)"; echo "Testing board $(BOARD)" >> test.log
	make test-board-config BOARD=$(BOARD) INIT_MEM=1 USE_DDR=0 RUN_DDR=0 TEST_LOG=1
	make test-board-config BOARD=$(BOARD) INIT_MEM=0 USE_DDR=0 RUN_DDR=0 TEST_LOG=1
ifeq ($(BOARD),AES-KU040-DB-G)
	make test-board-config BOARD=$(BOARD) INIT_MEM=0 USE_DDR=1 RUN_DDR=1 TEST_LOG=1
endif

test-all-boards:
	@rm -f test.log
	$(foreach b, $(BOARD_LIST), make test-board BOARD=$b;)
	diff -q test.log test/test-fpga.log
	@echo BOARD TEST PASSED FOR $(BOARD_LIST)


#
# COMPILE ASIC (WIP)
#

asic: asic-clean
	make -C $(FIRM_DIR) run BAUD=$(HW_BAUD)
	make -C $(BOOT_DIR) run BAUD=$(HW_BAUD)
	make -C $(SUBMODULES_DIR)/FSK_DEMOD demod_coeffs
ifeq ($(shell hostname), $(ASIC_SERVER))
	make -C $(ASIC_DIR) INIT_MEM=$(INIT_MEM) USE_DDR=$(USE_DDR) RUN_DDR=$(RUN_DDR) ASIC=1
else
	ssh $(ASIC_USER)@$(ASIC_SERVER) "if [ ! -d $(REMOTE_ROOT_DIR) ]; then mkdir -p $(REMOTE_ROOT_DIR); fi"
	rsync -avz --exclude .git $(ROOT_DIR) $(ASIC_USER)@$(ASIC_SERVER):$(REMOTE_ROOT_DIR)
	ssh -Y -C $(ASIC_USER)@$(ASIC_SERVER) 'cd $(REMOTE_ROOT_DIR); make -C $(ASIC_DIR) INIT_MEM=$(INIT_MEM) USE_DDR=$(USE_DDR) RUN_DDR=$(RUN_DDR) ASIC=1'
	scp $(ASIC_USER)@$(ASIC_SERVER):$(REMOTE_ROOT_DIR)/$(ASIC_DIR)/synth/*.log $(ASIC_DIR)/synth
	scp $(ASIC_USER)@$(ASIC_SERVER):$(REMOTE_ROOT_DIR)/$(ASIC_DIR)/synth/*.rpt $(ASIC_DIR)/synth
endif

asic-mem:
	make -C $(BOOT_DIR) run BAUD=$(HW_BAUD)
ifeq ($(shell hostname), $(ASIC_SERVER))
	make -C $(ASIC_DIR) mem INIT_MEM=$(INIT_MEM) USE_DDR=$(USE_DDR) RUN_DDR=$(RUN_DDR) ASIC=1
else
	ssh $(ASIC_USER)@$(ASIC_SERVER) "if [ ! -d $(REMOTE_ROOT_DIR) ]; then mkdir -p $(REMOTE_ROOT_DIR); fi"
	rsync -avz --exclude .git $(ROOT_DIR) $(ASIC_USER)@$(ASIC_SERVER):$(REMOTE_ROOT_DIR)
	ssh -Y -C $(ASIC_USER)@$(ASIC_SERVER) 'cd $(REMOTE_ROOT_DIR); make -C $(ASIC_DIR) mem INIT_MEM=$(INIT_MEM) USE_DDR=$(USE_DDR) RUN_DDR=$(RUN_DDR) ASIC=1'
endif

asic-synth:
	make -C $(SUBMODULES_DIR)/FSK_DEMOD demod_coeffs
ifeq ($(shell hostname), $(ASIC_SERVER))
	make -C $(ASIC_DIR) synth INIT_MEM=$(INIT_MEM) USE_DDR=$(USE_DDR) RUN_DDR=$(RUN_DDR) ASIC=1
else
	ssh $(ASIC_USER)@$(ASIC_SERVER) "if [ ! -d $(REMOTE_ROOT_DIR) ]; then mkdir -p $(REMOTE_ROOT_DIR); fi"
	rsync -avz --exclude .git $(ROOT_DIR) $(ASIC_USER)@$(ASIC_SERVER):$(REMOTE_ROOT_DIR)
	ssh -Y -C $(ASIC_USER)@$(ASIC_SERVER) 'cd $(REMOTE_ROOT_DIR); make -C $(ASIC_DIR) synth INIT_MEM=$(INIT_MEM) USE_DDR=$(USE_DDR) RUN_DDR=$(RUN_DDR) ASIC=1'
	scp $(ASIC_USER)@$(ASIC_SERVER):$(REMOTE_ROOT_DIR)/$(ASIC_DIR)/synth/*.log $(ASIC_DIR)/synth
	scp $(ASIC_USER)@$(ASIC_SERVER):$(REMOTE_ROOT_DIR)/$(ASIC_DIR)/synth/*.rpt $(ASIC_DIR)/synth
endif

asic-sim-synth: sw-clean
	make -C $(FIRM_DIR) run BAUD=$(SIM_BAUD) MODE=$(MODE)
	make -C $(BOOT_DIR) run BAUD=$(SIM_BAUD)
	make -C $(SUBMODULES_DIR)/FSK_DEMOD demod_coeffs
	make -C $(SUBMODULES_DIR)/FSK_DEMOD noise_floor.txt
ifeq ($(shell hostname), $(ASIC_SERVER))
	bash -c "trap 'make asic-sim-display-output' INT TERM KILL; make -C $(ASIC_DIR) sim INIT_MEM=$(INIT_MEM) USE_DDR=$(USE_DDR) RUN_DDR=$(RUN_DDR) TEST_LOG=$(TEST_LOG) VCD=$(VCD) DBG=$(DBG) BAUD=$(SIM_BAUD) SYNTH=1"
else
	ssh $(ASIC_USER)@$(ASIC_SERVER) "if [ ! -d $(REMOTE_ROOT_DIR) ]; then mkdir -p $(REMOTE_ROOT_DIR); fi"
	rsync -avz --exclude .git $(ROOT_DIR) $(ASIC_USER)@$(ASIC_SERVER):$(REMOTE_ROOT_DIR)
	bash -c "trap 'make kill-remote-asic-sim' INT TERM KILL; ssh -Y -C $(ASIC_USER)@$(ASIC_SERVER) 'cd $(REMOTE_ROOT_DIR); make -C $(ASIC_DIR) sim INIT_MEM=$(INIT_MEM) USE_DDR=$(USE_DDR) RUN_DDR=$(RUN_DDR) TEST_LOG=$(TEST_LOG) VCD=$(VCD) DBG=$(DBG) BAUD=$(SIM_BAUD) SYNTH=1'"
	scp $(ASIC_USER)@$(ASIC_SERVER):$(REMOTE_ROOT_DIR)/$(ASIC_DIR)/sim/soc*.txt $(ASIC_DIR)/sim
ifeq ($(VCD),1)
	scp $(ASIC_USER)@$(ASIC_SERVER):$(REMOTE_ROOT_DIR)/$(ASIC_DIR)/*.vcd $(ASIC_DIR)
endif
endif
	make asic-sim-display-output

asic-sim-display-output:
	@echo ""
	@echo "#"
	@echo "# Soc 0"
	@echo "#"
	@echo ""
	@cat $(ASIC_DIR)/sim/soc0.txt
	@echo ""
	@echo "#"
	@echo "# Soc 1"
	@echo "#"
	@echo ""
	@cat $(ASIC_DIR)/sim/soc1.txt

kill-remote-asic-sim:
	@echo "INFO: Remote simulator will be killed; ignore following errors"
	ssh $(ASIC_USER)@$(ASIC_SERVER) 'cd $(REMOTE_ROOT_DIR); killall -q -u $(ASIC_USER) -9 xmsim'
	scp $(ASIC_USER)@$(ASIC_SERVER):$(REMOTE_ROOT_DIR)/$(ASIC_DIR)/sim/soc*.txt $(ASIC_DIR)/sim
	make asic-sim-display-output

asic-clean: sw-clean hex-clean
	make -C $(ASIC_DIR) clean
ifneq ($(shell hostname), $(ASIC_SERVER))
	rsync -avz --exclude .git $(ROOT_DIR) $(ASIC_USER)@$(ASIC_SERVER):$(REMOTE_ROOT_DIR)
	ssh $(ASIC_USER)@$(ASIC_SERVER) 'if [ -d $(REMOTE_ROOT_DIR) ]; then cd $(REMOTE_ROOT_DIR); make -C $(ASIC_DIR) clean; fi'
endif


# CLEAN ALL
clean-all: sim-clean fpga-clean asic-clean board-clean doc-clean

.PHONY: pc-emul pc-clean \
	sim sim-waves sim-display-output kill-remote-sim sim-clean \
	fpga fpga-clean fpga-clean-ip \
	board-load board-run kill-remote-console board-clean \
	firmware firmware-clean bootloader bootloader-clean sw-clean \
	console console-clean \
	hex-clean \
	doc doc-clean doc-pdfclean \
	test test-all-simulators test-simulator test-all-boards test-board test-board-config \
	asic asic-mem asic-synth asic-sim-synth asic-sim-display-output kill-remote-asic-sim asic-clean \
	all clean-all
