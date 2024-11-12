# Author: Jaleel Rogers

COMP = g++
COMPFLAGS = -std=c++11
TARGET = jurassicScheduler
SRC = jurassicScheduler.cpp

all: $(TARGET)


$(TARGET): $(SRC)
	@echo "Compiling $(TARGET)..."
	sudo $(COMP) $(COMPFLAGS) $(SRC) -o $(TARGET)

clean:
	@echo "Cleaning up..."
	rm -f $(TARGET)

install: $(TARGET)
	@echo "Installing $(TARGET)..."
	sudo cp $(TARGET)

uninstall:
	@echo "Uninstalling $(TARGET)..."
	sudo rm -f $(TARGET)
