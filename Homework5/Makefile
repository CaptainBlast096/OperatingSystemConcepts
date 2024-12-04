# Author: Jaleel Rogers

COMP = gcc
CFLAGS = -pthread
TARGET = CretaceousWorkloadBalancer
SRC = CretaceousWorkloadBalancer.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(COMP) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

install: $(TARGET)
	cp $(TARGET)
