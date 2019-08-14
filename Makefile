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

all: seccomp.so
	@echo "run 'make install' or 'make uninstall'"

seccomp.so: seccomp.c
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) $(LFLAGS) -lseccomp -shared seccomp.c -o seccomp.so

install: seccomp.so $(PROGS)
	$(Q)for prog in ${PROGS}; do \
		path=${BIN_DIR}/$$prog; \
		echo "CP $$prog $$path"; \
		cp $$prog $$path; \
	done

	@path=${LIB_DIR}/seccomp.so; \
	echo "CP seccomp.so $$path"; \
	cp seccomp.so $$path;

uninstall:
	$(Q)for prog in ${PROGS}; do \
		path=${BIN_DIR}/$$prog; \
		echo "RM $$path"; \
		rm $$path; \
	done

clean:
	@echo "CLEAN"
	$(Q)rm -f *.o *.so
