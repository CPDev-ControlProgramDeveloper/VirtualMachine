#
# Makefile for cc65
#
# This Makefile requires GNU make
#

# Run 'make SYS=<target>'; or, set a SYS env.
# var. to build for another target system.
SYS ?= z80

# --------------------------------------------------------------------------
# Lists of source directories
TARGET = test
SRCDIR = vm vm/vmfunc 
SOURCES = $(wildcard *.c $(foreach fd, $(SRCDIR), $(fd)/*.c))
OBJDIR = ./obj
OBJS = $(addprefix $(OBJDIR)/, $(SOURCES:c=rel))
OUTDIR = ./bin

# --------------------------------------------------------------------------
TARGET_FILE = $(TARGET)

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

CC = sdcc
CL = sdcc
LD = sdcc

# --------------------------------------------------------------------------
# Generic rules

.PHONY: $(TARGET_FILE)

$(OUTDIR)/$(TARGET_FILE): $(OBJS)
	mkdir -p $(OUTDIR)
	$(LD) $(LDFLAGS_$(@F)_$(SYS)) $(LDFLAGS) -m$(SYS) $^ -L $(SYS).lib -o $@ 

$(OBJDIR)/%.rel: %.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -m$(SYS) -c -o $(OBJDIR)/$(<:c=rel) $<
#	$(AS) $(addprefix $(OBJDIR)/, $(<:.c=.s))

# --------------------------------------------------------------------------
# Clean-up rules

clean:
	rm -rf $(OUTDIR)/* $(OBJDIR)/*
