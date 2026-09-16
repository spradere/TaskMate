# Compiler and Programmer
CC = avr-gcc
OBJCOPY = avr-objcopy
AVRDUDE = avrdude

# MCU and Programmer settings
MCU = atmega2560
F_CPU = 16000000UL
PROGRAMMER = avrispmkII
PORT = /dev/ttyU0

# Files
TARGET = TaskMate0.2
SRC = $(TARGET).c task1.c task2.c
OBJ = $(SRC:.c=.o) 
HEX = $(TARGET).hex
ELF = $(TARGET).elf

# Compiler flags
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall

# Build rules

all:$(OBJ)
	#implicit build in rule *.c -> *.o

	#link objects
	$(CC) $(CFLAGS) -o $(ELF) $(OBJ)
	
upload:all
	#to hex format
	$(OBJCOPY) -O ihex -R .eeprom $(ELF) $(HEX)
	#upload to atmega
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU) -U flash:w:$(HEX):i -P $(PORT) -D

	
clean:
	rm -f $(ELF) $(HEX) $(OBJ)
