-include config.mk

KWARCDIR := $(realpath $(dir $(CURDIR)))
ifdef SystemDrive
	EXE := .exe
endif
KWARC := $(KWARCDIR)/KWARC$(EXE)

CFLAGS 	+= -g

TESTSPECS  := $(patsubst %.spec,%.c,$(wildcard *.spec))
TESTCASES  := $(wildcard test_*.c)
TESTOBJS   := $(patsubst %.c,%.o,$(TESTSPECS) $(TESTCASES))
TESTRUNNER := tests$(EXE)

all: $(TESTRUNNER)

$(TESTRUNNER): $(TESTOBJS) test.o tests.o
	$(CC) $(LDFLAGS) $^ -o $@

http_headers.c: $(KWARC) http_headers.spec
	$(KWARC) -i -d = $(filter %.spec,$^)

%.c: $(KWARC) %.spec
	$(KWARC) -i $(filter %.spec,$^)

$(KWARC):
	$(MAKE) -C $(KWARCDIR)

clean:
	$(RM) *.o $(TESTSPECS) $(patsubst %.c,%.h,$(TESTSPECS)) $(TESTRUNNER)

.SECONDARY: $(TESTSPECS)
