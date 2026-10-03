XC      ?= /opt/microchip/xc-dsc/v4.00
DFP     ?= $(HOME)/.local/microchip/packs/dsPIC33CK-MC_DFP/1.11.412
CC       = $(XC)/bin/xc-dsc-gcc
BIN2HEX  = $(XC)/bin/xc-dsc-bin2hex

MCU      = 33CK64MC105
TARGET   = hello
BUILD    = build
SRCS     = main.c
OBJS     = $(SRCS:%.c=$(BUILD)/%.o)

CFLAGS   = -mcpu=$(MCU) -mdfp=$(DFP)/xc16 -omf=elf -O1 -Wall -std=gnu99
LDFLAGS  = -mcpu=$(MCU) -mdfp=$(DFP)/xc16 -omf=elf -Wl,--script=p$(MCU).gld,--heap=0,-Map=$(BUILD)/$(TARGET).map

CURIOSITY ?= /run/media/$(USER)/CURIOSITY
PORT      ?= /dev/ttyACM0
BAUD      ?= 9600

.PHONY: all flash monitor clean

all: $(BUILD)/$(TARGET).hex

$(BUILD)/%.o: %.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/$(TARGET).elf: $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

$(BUILD)/$(TARGET).hex: $(BUILD)/$(TARGET).elf
	$(BIN2HEX) -mdfp=$(DFP)/xc16 -omf=elf $<

$(BUILD):
	mkdir -p $@

# Programación por arrastrar y soltar: copiar el .hex al disco del depurador
flash: $(BUILD)/$(TARGET).hex
	cp $< $(CURIOSITY)/ && sync
	@cat $(CURIOSITY)/STATUS.TXT 2>/dev/null || true

monitor:
	python3 -m serial.tools.miniterm $(PORT) $(BAUD)

clean:
	rm -rf $(BUILD)
