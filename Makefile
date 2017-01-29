#
# Makefile
# Copyright © 2017 Samuel Holland <samuel@sholland.org>
# See LICENSE in the project directory for license terms.
# vim: ft=make:noexpandtab:sts=4:sw=4:ts=4:tw=100
#

SRCDIR   := .
SUFFIX   :=

-include config.mk

CFLAGS   += -std=c11 -Wall -Wextra
CPPFLAGS += -D_POSIX_C_SOURCE=200809L -I$(srcdir)/include -Ibuild/generated/include
LDFLAGS  +=

ifneq ($(DEBUG),)
CFLAGS   += -g -Werror -Wpedantic
else
CPPFLAGS += -DNDEBUG
LDFLAGS  += -s
endif

BINARIES := 
DOCS     := LICENSE README
HEADERS  := $(wildcard $(SRCDIR)/include/*/*.h) build/generated/include/version.h
LIBRARIES:= 
VERSION  := 1.0-dev

BINSRCS  := $(addprefix $(SRCDIR)/src/,$(addsuffix .c,$(BINARIES)))
BINOBJS  := $(patsubst $(SRCDIR)/src/%.c,build/obj/%.o,$(BINSRCS))
BINFILES := $(addprefix build/bin/,$(addsuffix $(SUFFIX),$(BINARIES)))

LIBSRCS  := $(filter-out $(BINSRCS),$(wildcard $(SRCDIR)/src/*.c))
LIBOBJS  := $(patsubst $(SRCDIR)/src/%.c,build/obj/%.o,$(LIBSRCS))
LIBFILES := $(addprefix build/lib/lib,$(addsuffix .a,$(LIBRARIES)))

TESTSRCS := $(wildcard $(SRCDIR)/test/*.c)
TESTOBJS := $(patsubst $(SRCDIR)/test/%.c,build/test/%.o,$(TESTSRCS))
TESTBINS := $(basename $(TESTOBJS))
TESTFILES:= $(addsuffix .out,$(TESTBINS))

all: $(BINFILES) $(LIBFILES) $(TESTFILES)

bin: $(BINFILES)

check: $(TESTFILES)

clean:
	@printf ' %-6s %s\n' CLEAN build
	@rm -rf build

format: $(HEADERS) $(BINSRCS) $(LIBSRCS) $(TESTSRCS)
	@clang-format -i $^

lib: $(LIBFILES)

build/bin build/generated/include build/lib build/obj build/test:
	@mkdir -p $@

build/bin/%: build/obj/%.o $(LIBFILES) | build/bin
	@printf ' %-6s %s\n' CCLD "$@"
	@$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

build/generated/include/version.h: Makefile | build/generated/include
	@printf ' %-6s %s\n' GEN "$@"
	@printf '#define VERSION "%s"\n' "$(VERSION)" > $@

build/lib/lib%.a: $(LIBOBJS) | build/lib
	@printf ' %-6s %s\n' AR "$@"
	@$(AR) rcs $@ $^

build/obj/%.o: $(SRCDIR)/src/%.c $(HEADERS) | build/obj
	@printf ' %-6s %s\n' CC "$@"
	@$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

build/test/%: build/test/%.o $(LIBFILES) | build/test
	@printf ' %-6s %s\n' CCLD "$@"
	@$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

build/test/%.o: $(SRCDIR)/test/%.c $(HEADERS) | build/test
	@printf ' %-6s %s\n' CC "$@"
	@$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

build/test/%.out: build/test/% | build/test
	@printf ' %-6s %s\n' TEST "$<"
	@./$< > $@

.PHONY: all bin check clean format lib
.SECONDARY: $(BINOBJS) $(LIBOBJS) $(TESTBINS) $(TESTOBJS)
