PROGS = sbox
DIR = /usr/bin

.PHONY: install uninstall all

all:
	@echo "'make install' or 'make uninstall'"

install:
	@for prog in ${PROGS}; do \
		path=${DIR}/$$prog; \
		echo "CP $$prog $$path"; \
		cp $$prog $$path; \
	done

uninstall:
	@for prog in ${PROGS}; do \
		path=${DIR}/$$prog; \
		echo "RM $$path"; \
		rm $$path; \
	done
