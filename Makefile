CC = gcc
CFLAGS = $(shell pkg-config --cflags gtk+-3.0 gtksourceview-3.0)
LIBS = $(shell pkg-config --libs gtk+-3.0 gtksourceview-3.0)
TARGET = main

all: $(TARGET)

$(TARGET): main.c gui/gui.c
	$(CC) $(CFLAGS) -o $(TARGET) main.c gui/gui.c $(LIBS)

clean:
	rm -f $(TARGET)

.PHONY: all clean