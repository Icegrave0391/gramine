# Copyright (C) 2023 Gramine contributors
# SPDX-License-Identifier: BSD-3-Clause

ARCH_LIBDIR ?= /lib/$(shell $(CC) -dumpmachine)

ifeq ($(DEBUG),1)
GRAMINE_LOG_LEVEL = debug
else
GRAMINE_LOG_LEVEL = error
endif

UNQLITE_DIR = unqlite
EXAMPLE_DIR = unqlite/example

.PHONY: all
all: unqlite.manifest sub-make
# ifeq ($(SGX),1)
# all: bash.manifest.sgx bash.sig
# endif

unqlite.manifest: unqlite.manifest.template
	gramine-manifest \
		-Dlog_level=$(GRAMINE_LOG_LEVEL) \
		-Dexecdir=$(shell pwd)/$(EXAMPLE_DIR) \
		-Darch_libdir=$(ARCH_LIBDIR) \
		$< >$@
	
sub-make:
	cd $(UNQLITE_DIR) && cmake . && make

# bash.manifest.sgx: bash.manifest
# 	gramine-sgx-sign \
# 		--manifest bash.manifest \
# 		--output $@

# bash.sig: bash.manifest.sgx

ifeq ($(SGX),)
GRAMINE = gramine-encos
# else
# GRAMINE = gramine-sgx
endif


.PHONY: clean
clean:
	$(RM) *.manifest *.manifest.sgx *.token *.sig OUTPUT scripts/testdir/*

.PHONY: distclean
distclean: clean
