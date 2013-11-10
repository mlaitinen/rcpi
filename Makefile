CC=gcc # Compiler
CFLAGS=-I. -Wall # Compiler flags
OUTPUT=rc # Output file name
LIBS=-lpthread -lpigpio -lrt # External libraries

rcmake: joystick.c keyboard.c main.c
	$(CC) -o $(OUTPUT) joystick.c keyboard.c main.c $(CFLAGS) $(LIBS)
