# Compiler 
CC = avr-gcc

# MCU and Programmer settings
MCU = atmega2560
F_CPU = 16000000UL
PROGRAMMER = avrispmkII
PORT = /dev/ttyU0

# Files
TARGET = TaskMate
SRC = sysCore/$(TARGET).c sysCore/sysCall.c sysCore/taskCreate.c drivers/timer1D.c tasks/task1.c tasks/task2.c
OBJ = $(SRC:.c=.o) 
HEX = $(TARGET).hex
ELF = $(TARGET).elf

# Compiler flags
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -O2 -Wall -I/root/code/TaskMate/TaskMate_current

# Build rules

all:$(OBJ)
	#implicit build in rule *.c -> *.o

	#link objects
	$(CC) $(CFLAGS) -o $(ELF) $(OBJ)
	
upload:all
	#to hex format
	avr-objcopy -O ihex -R .eeprom $(ELF) $(HEX)
	#upload to atmega
	avrdude -c $(PROGRAMMER) -p $(MCU) -U flash:w:$(HEX):i -P $(PORT) -D

	
clean:
	rm -f $(ELF) $(HEX) $(OBJ) *.out
	
dump:all
	avr-objcopy -O ihex -R .eeprom $(ELF) $(HEX)
	avr-objdump -D -m avr6 $(HEX) > hex.out
	avr-objdump -D -m avr6 $(ELF) > elf.out
	
