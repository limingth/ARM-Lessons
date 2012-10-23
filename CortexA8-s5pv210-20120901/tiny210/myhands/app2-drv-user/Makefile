CC = arm-linux-gcc
CFLAGS = -Wall 
LDFLAGS = 

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
DIRNAME = $(notdir $(PWD))

TARGET = $(DIRNAME)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $@

clean:
	-rm $(TARGET)
	-rm *.o




