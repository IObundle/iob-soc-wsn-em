CC = soffice
CFLAGS = --headless --convert-to png 
LO_RUNNING = $(shell pgrep soffice)
OBJS = $(patsubst %.odg,%.eps,$(wildcard *.odg))

all: $(OBJS)

ifeq ($(LO_RUNNING), )
%.eps: %.odg
	$(CC) $(CFLAGS) $<
else 	
$(error LibreOffice is running. Close it before proceeding.)
endif

clean:
	@rm -f *.png *.pdf

.PHONY: clean all

