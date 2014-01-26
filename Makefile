CC=gcc # Compiler
CFLAGS=-I. -Wall # Compiler flags
OUTPUT=rcpi # Output file name
LIBS=-lpthread -lpigpio -lrt # External libraries

rcmake: joystick.c keyboard.c util.c main.c
	$(CC) -o $(OUTPUT) joystick.c keyboard.c udp.c util.c main.c $(CFLAGS) $(LIBS)
