OBJS = $(patsubst %.odg,%.png,$(wildcard ./drawings/*.odg))
WAVES = $(wildcard ./wavedrom/*.png)

TRGT = rep
SRC =  intro.tex 

all: $(TRGT).pdf

$(TRGT).pdf: $(TRGT).tex $(OBJS) $(SRC) $(WAVES) $(TRGT).cls
	latexmk -pdf $(TRGT).tex


./drawings/%.png: ./drawings/%.odg
	make -C ./drawings

clean:
	@rm -f $(addprefix rep, .lof .toc .aux .out .lot .log .bbl .blg .fls .fdb_latexmk) *~
	make -C ./drawings clean

.PHONY: clean all

