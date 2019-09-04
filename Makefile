PROGS = sbox sbox-fstool
BIN_DIR = /usr/bin
LIB_DIR = /usr/lib
CC = gcc
CFLAGS  = -Wall -Werror
LFLAGS = -I./include

ifneq ($(V),1)
Q = @
endif

.PHONY: install uninstall all

all: sbox-seccomp.so sbox-aa.so
	@echo "run 'make install' or 'make uninstall'"

sbox-seccomp.so: sbox-seccomp.c
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) $(LFLAGS) -lseccomp -shared sbox-seccomp.c -o sbox-seccomp.so

sbox-aa.so: sbox-aa.c
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) $(LFLAGS) -lapparmor -shared sbox-aa.c -o sbox-aa.so

install: sbox-seccomp.so sbox-aa.so $(PROGS)
	$(Q)for prog in ${PROGS}; do \
		path=${BIN_DIR}/$$prog; \
		echo "CP $$prog $$path"; \
		cp $$prog $$path; \
	done

	@path=${LIB_DIR}/sbox-seccomp.so; \
	echo "CP sbox-seccomp.so $$path"; \
	cp sbox-seccomp.so $$path; \

	@path=${LIB_DIR}/sbox-aa.so; \
	echo "CP sbox-aa.so $$path"; \
	cp sbox-aa.so $$path; \

uninstall:
	$(Q)for prog in ${PROGS}; do \
		path=${BIN_DIR}/$$prog; \
		echo "RM $$path"; \
		rm $$path; \
	done
	@echo "RM sbox-aa.so"
	$(Q)rm ${LIB_DIR}/sbox-aa.so
	@echo "RM sbox-seccomp.so"
	$(Q)rm ${LIB_DIR}/sbox-seccomp.so

clean:
	@echo "CLEAN"
	$(Q)rm -f *.o *.so
