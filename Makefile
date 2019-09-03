PROGS = sbox sbox-fstool
SHARED_OBJS = sbox-seccomp.so sbox-aa.so sbox-io.so
BIN_DIR = /usr/bin
LIB_DIR = /usr/lib
CC = gcc
CFLAGS  = -Wall -Werror
LFLAGS = -I./include

ifneq ($(V),1)
Q = @
endif

.PHONY: install uninstall all

all: $(SHARED_OBJS)
	@echo "run 'make install' or 'make uninstall'"

sbox-seccomp.so: sbox-seccomp.c
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) $(LFLAGS) -lseccomp -shared sbox-seccomp.c -o sbox-seccomp.so

sbox-aa.so: sbox-aa.c
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) $(LFLAGS) -lapparmor -shared sbox-aa.c -o sbox-aa.so

sbox-io.so: sbox-io.c
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) $(LFLAGS) -shared sbox-io.c -o sbox-io.so

install: $(SHARED_OBJS) $(PROGS)
	$(Q)for prog in ${PROGS}; do \
		path=${BIN_DIR}/$$prog; \
		echo "CP $$prog $$path"; \
		cp $$prog $$path; \
	done \

	$(Q)for so in ${SHARED_OBJS}; do \
		path=${LIB_DIR}/$$so; \
		echo "CP $$so $$path"; \
		cp $$so $$path; \
	done \

uninstall:
	$(Q)for prog in ${PROGS}; do \
		path=${BIN_DIR}/$$prog; \
		echo "RM $$path"; \
		rm $$path; \
	done \

	$(Q)for so in ${SHARED_OBJS}; do \
		path=${LIB_DIR}/$$so; \
		echo "RM $$path"; \
		rm $$path; \
	done \

clean:
	@echo "CLEAN"
	$(Q)rm -f *.o *.so
