#
# Makefile
# Copyright © 2016-2017 Samuel Holland <samuel@sholland.org>
# See LICENSE in the project directory for license terms.
# vim: ft=make:noexpandtab:sts=4:sw=4:ts=4:tw=100
#

SRCDIR  := .
SUFFIX  :=

-include config.mk

AUTHOR  := Samuel Holland <samuel@sholland.org>
PROJECT :=
VERSION := 1.0-dev

CFLAGS  += -std=c11 -Wall -Werror=implicit-function-declaration -Werror=implicit-int -Wextra
CPPFLAGS+= -D_POSIX_C_SOURCE=200809L -I$(SRCDIR)/include -Ibuild/generated/include
LDFLAGS +=

ifneq ($(DEBUG),)
CFLAGS  += -g -Werror -Wpedantic
else
CPPFLAGS+= -DNDEBUG
LDFLAGS += -s
endif

BINSRCS := $(wildcard $(SRCDIR)/src/*.c)
BINOBJS := $(patsubst $(SRCDIR)/src/%.c,build/obj/%.o,$(BINSRCS))
BINARIES:= $(patsubst $(SRCDIR)/src/%.c,build/bin/%$(SUFFIX),$(BINSRCS))

HEADERS := $(wildcard $(SRCDIR)/include/*/*.h) build/generated/include/version.h

LIBSRCS := $(wildcard $(SRCDIR)/lib/*.c)
LIBOBJS := $(patsubst $(SRCDIR)/lib/%.c,build/lib/%.o,$(LIBSRCS))
LIBRARY := build/lib/lib$(PROJECT).a

TESTSRCS:= $(wildcard $(SRCDIR)/test/*.c)
TESTOBJS:= $(patsubst $(SRCDIR)/test/%.c,build/test/%.o,$(TESTSRCS))
TESTBINS:= $(addsuffix $(SUFFIX),$(basename $(TESTOBJS)))
TESTOUTS:= $(patsubst %$(SUFFIX),%.out,$(TESTBINS))

M := @printf ' %-6s %s\n'
Q := @
ifneq ($(V),)
M := @\#
Q :=
endif

all: $(BINARIES) $(LIBRARY) $(TESTOUTS)

bin: $(BINARIES)

check: $(TESTOUTS)

clean:
	$(M) CLEAN build
	$(Q) rm -rf build

format: $(HEADERS) $(BINSRCS) $(LIBSRCS) $(TESTSRCS)
	$(Q) clang-format -i $^

lib: $(LIBRARY)

build/bin build/generated/include build/lib build/obj build/test:
	$(Q) mkdir -p $@

build/bin/%$(SUFFIX): build/obj/%.o $(LIBRARY) | build/bin
	$(M) CCLD '$@'
	$(Q) $(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< -Lbuild/lib -l$(PROJECT)

build/generated/include/version.h: Makefile | build/generated/include
	$(M) GEN '$@'
	$(Q) printf '#define AUTHOR "%s"\n#define VERSION "%s"\n' "$(AUTHOR)" "$(VERSION)" > $@

build/lib/lib$(PROJECT).a: $(LIBOBJS) | build/lib
	$(M) AR '$@'
	$(Q) $(AR) rcs $@ $^

build/lib/%.o: $(SRCDIR)/lib/%.c $(HEADERS) | build/lib
	$(M) CC '$@'
	$(Q) $(CC) -fPIC $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

build/obj/%.o: $(SRCDIR)/src/%.c $(HEADERS) | build/obj
	$(M) CC '$@'
	$(Q) $(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

build/test/%$(SUFFIX): build/test/%.o $(LIBRARY) | build/test
	$(M) CCLD '$@'
	$(Q) $(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< -Lbuild/lib -l$(PROJECT)

build/test/%.o: $(SRCDIR)/test/%.c $(HEADERS) | build/test
	$(M) CC '$@'
	$(Q) $(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

build/test/%.out: build/test/%$(SUFFIX) | build/test
	$(M) TEST "$<"
	$(Q) ./$< > $@

.PHONY: all bin check clean format lib
.SECONDARY: $(BINOBJS) $(LIBOBJS) $(TESTBINS) $(TESTOBJS)
