-include config.mk

ifdef SystemDrive
	EXE := .exe
endif
KWARC := kwarc$(EXE)

CFLAGS ?= -O2

SRC := $(wildcard *.c)
OBJ := $(patsubst %.c,%.o,$(SRC))

all: $(KWARC)

$(KWARC): $(OBJ)
	$(CC) $(LDFLAGS) $^ -o $@

clean:
	$(RM) *.o $(KWARC)
	$(MAKE) -C tests clean
