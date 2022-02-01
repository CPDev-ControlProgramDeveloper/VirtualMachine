#
# Makefile for cc65
#
# This Makefile requires GNU make
#

# Run 'make SYS=<target>'; or, set a SYS env.
# var. to build for another target system.
SYS ?= c64

# --------------------------------------------------------------------------
# Lists of source directories
TARGET = test
SRCDIR = vm vm/vmfunc 
SOURCES = $(wildcard *.c $(foreach fd, $(SRCDIR), $(fd)/*.c))
OBJDIR = ./obj
OBJS = $(addprefix $(OBJDIR)/, $(SOURCES:c=o))
OUTDIR = ./bin

# --------------------------------------------------------------------------
ifeq ($(SYS),c64)
TARGET_FILE = $(addsuffix .prg, $(TARGET))
else 
ifeq ($(SYS),atari)
TARGET_FILE = $(addsuffix .xex, $(TARGET))
else
TARGET_FILE = $(TARGET)
endif
endif

# Just the usual way to find out if we're
# using cmd.exe to execute make rules.
ifneq ($(shell echo),)
  CMD_EXE = 1
endif

ifdef CMD_EXE
  NULLDEV = nul:
  DEL = -del /f
  RMDIR = rmdir /s /q
else
  NULLDEV = /dev/null
  DEL = $(RM)
  RMDIR = $(RM) -r
endif

ifdef CC65_HOME
  AS = $(CC65_HOME)/bin/ca65
  CC = $(CC65_HOME)/bin/cc65
  CL = $(CC65_HOME)/bin/cl65
  LD = $(CC65_HOME)/bin/ld65
else
  AS := $(if $(wildcard ../bin/ca65*),../bin/ca65,ca65)
  CC := $(if $(wildcard ../bin/cc65*),../bin/cc65,cc65)
  CL := $(if $(wildcard ../bin/cl65*),../bin/cl65,cl65)
  LD := $(if $(wildcard ../bin/ld65*),../bin/ld65,ld65)
endif

# --------------------------------------------------------------------------
# Generic rules

.PHONY: $(TARGET_FILE)

$(OUTDIR)/$(TARGET_FILE): $(OBJS)
	mkdir -p $(OUTDIR)
ifeq ($(SYS),vic20)
 	$(LD) $(LDFLAGS_$(@F)_$(SYS)) $(LDFLAGS) -o $@ -C vic20-32k.cfg -m $@.map $^ $(SYS).lib
else
	$(LD) $(LDFLAGS_$(@F)_$(SYS)) $(LDFLAGS) -o $@ -t $(SYS) -m $@.map $^ $(SYS).lib
endif

$(OBJDIR)/%.o: %.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -Ors --codesize 500 -T -g -t $(SYS) -o $(@:o=s) $<
	$(AS) $(addprefix $(OBJDIR)/, $(<:.c=.s))

# --------------------------------------------------------------------------
# Clean-up rules

clean:
	rm -rf $(OUTDIR)/* $(OBJDIR)/*
