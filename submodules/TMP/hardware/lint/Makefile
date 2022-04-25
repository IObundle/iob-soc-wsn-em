TMP_DIR:=../..

incdir:=-I
defmacro:=-D

include ../hardware.mk

VFLAGS = --lint-only -Wall
LINT_LOG=lint_veri

.PHONY: run run_lint clean

run: clean run_lint

run_lint: $(VHDR) $(VSRC)
	verilator $(VFLAGS) $(INCLUDE) $(DEFINE) $(VSRC) 2<&1 | tee $(LINT_LOG).log

clean:
	@rm -f *.log
