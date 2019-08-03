PROGS = sbox sbox-fstool
DIR = /usr/bin

.PHONY: install uninstall all

all:
	@echo "run 'make install' or 'make uninstall'"

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
