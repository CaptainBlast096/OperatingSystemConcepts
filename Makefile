#Makefile
#Author: Jaleel Rogers

#Assigned values to variables to make it easier to change later
COMPILER = gcc
TARGET = program
OBJECTS = LuckyNumber.o

$(TARGET): main.c $(OBJECTS)
	$(COMPILER) -o $(TARGET) main.c $(OBJECTS)

LuckyNumber.o: LuckyNumber.c LuckyNumber.h
	$(COMPILER) -c LuckyNumber.c

clean:
	rm -f $(TARGET) $(OBJECTS)

install:
	sudo cp $(TARGET) /usr/local/bin/

uninstall:
	sudo rm -f /usr/local/bin/$(TARGET)
