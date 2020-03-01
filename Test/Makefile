CC := mpic++

TARGET := main
INC_DIR := "/usr/include/x86_64-linux-gnu/mpich/"

.PHONY: all clean

all: $(TARGET)

$(TARGET): main.o
	$(CC) $^ -I$(INC_DIR) -o $@

main.o: main.cpp
	$(CC) -c $<

clean:
	$(RM) main.o
