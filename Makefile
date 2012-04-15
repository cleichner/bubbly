# Hey Emacs, this is a -*- makefile -*-
# based on the WinAVR Makefile Template written by Eric B. Weddington, Jörg
# Wunsch, et al.
#
# make all = make everything
# make clean = remove build files
# make program = Download the hex file to the device, using avrdude.
# make filename.s = compile filename.c into assembly

# Define programs and commands.
CC = gcc
AVR_CC = avr-gcc
SHELL = sh
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size
AR = avr-ar rcs
NM = avr-nm
AVRDUDE = avrdude
REMOVE = rm -f
REMOVEDIR = rm -rf
COPY = cp

MSG_ERRORS_NONE = Errors: none
MSG_SIZE_BEFORE = Size before:
MSG_SIZE_AFTER = Size after:
MSG_COFF = Converting to AVR COFF:
MSG_EXTENDED_COFF = Converting to AVR Extended COFF:
MSG_FLASH = Creating load file for Flash:
MSG_EEPROM = Creating load file for EEPROM:
MSG_EXTENDED_LISTING = Creating Extended Listing:
MSG_SYMBOL_TABLE = Creating Symbol Table:
MSG_LINKING = Linking:
MSG_COMPILING = Compiling C:
MSG_ASSEMBLING = Assembling:
MSG_CLEANING = Cleaning project:
MSG_CREATING_LIBRARY = Creating library:

MCU = atmega168
F_CPU = 8000000 # not strictly true

# Output format.
FORMAT = ihex

# Optimization [0, 1, 2, 3, s]
OPT = s

CSTANDARD = -std=gnu99
CFLAGS =  -Wall
CFLAGS += -O$(OPT)
CFLAGS += -Werror
CFLAGS += -Wextra
CFLAGS += -pedantic
CFLAGS += -fpack-struct
CFLAGS += -Wstrict-prototypes
CFLAGS += -Wsign-compare
CFLAGS += $(CSTANDARD)

PRINTF_LIB_MIN = -Wl,-u,vfprintf -lprintf_min
PRINTF_LIB_FLOAT = -Wl,-u,vfprintf -lprintf_flt
SCANF_LIB_MIN = -Wl,-u,vfscanf -lscanf_min
SCANF_LIB_FLOAT = -Wl,-u,vfscanf -lscanf_flt
MATH_LIB = -lm
EXTRALIBDIRS =

TARGET = bubbly
AVR_LDFLAGS = -Wl,-Map=$(TARGET).map,--cref

AVRDUDE_PROGRAMMER = avrispmkII
AVRDUDE_PORT = lpt1
AVRDUDE_WRITE_FLASH = -U flash:w:$(TARGET).hex
AVRDUDE_VERBOSE = -v -v

AVRDUDE_FLAGS = -p $(MCU) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER)
AVRDUDE_FLAGS += $(AVRDUDE_VERBOSE)
AVR_CFLAGS = -mmcu=$(MCU) -I. $(CFLAGS)

all: gccversion build simulation

# Program the device.
program: $(TARGET).hex $(TARGET).eep
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH) $(AVRDUDE_WRITE_EEPROM)

# Create final output files (.hex, .eep) from ELF output file.
%.hex: %.elf
	@echo
	@echo $(MSG_FLASH) $@
	$(OBJCOPY) -O $(FORMAT) -R .eeprom $< $@

%.eep: %.elf
	@echo
	@echo $(MSG_EEPROM) $@
	-$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 --no-change-warnings -O $(FORMAT) $< $@ || exit 0

# Target: clean project.
clean: begin clean_list end

clean_list :
	@echo
	@echo $(MSG_CLEANING)
	$(REMOVE) $(TARGET).hex
	$(REMOVE) $(TARGET).eep
	$(REMOVE) $(TARGET).cof
	$(REMOVE) $(TARGET).elf
	$(REMOVE) $(TARGET).map
	$(REMOVE) $(TARGET).sym
	$(REMOVE) $(TARGET).lss
	$(REMOVE) $(SRC:%.c=$(OBJDIR)/%.o)
	$(REMOVE) $(SRC:%.c=$(OBJDIR)/%.lst)
	$(REMOVE) $(SRC:.c=.s)
	$(REMOVE) $(SRC:.c=.d)
	$(REMOVE) $(SRC:.c=.i)
	$(REMOVEDIR) .dep

simulation:
	$(CC) $(CFLAGS) -c -Ihardware -I. hardware/simulated/display_ncurses.c
	$(CC) $(CFLAGS) -c -Ihardware -I. hardware/simulated/display_text.c
	$(CC) $(CFLAGS) -c -Ihardware -I. hardware/simulated/hardware_sim.c
	$(CC) $(CFLAGS) -c -Ihardware -I. bubbly.c
	$(CC) $(CFLAGS) -c -Ihardware -I. main.c
	$(CC) -lcurses -o softbot bubbly.o hardware_sim.o display_ncurses.o main.o
	$(CC) -lcurses -o textbot bubbly.o hardware_sim.o display_text.o main.o

clean:
	rm -f *.o softbot textbot

.PHONY : all begin finish end sizebefore sizeafter gccversion \
build elf hex eep lss sym coff extcoff \
clean clean_list program debug gdb-config simulation
