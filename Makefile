CC = gcc
CFLAGS = $(shell pkg-config --cflags gtk+-3.0 gtksourceview-3.0)
LIBS = $(shell pkg-config --libs gtk+-3.0 gtksourceview-3.0)
TARGET = main

LANG_SRCS = lang/lang.c lang/parser.tab.c lang/lex.yy.c

SRCS = main.c gui/gui.c $(LANG_SRCS)

all: $(TARGET)

# regla principal: genera bison/flex si es necesario (ver abajo)
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) main.c gui/gui.c $(LANG_SRCS) $(LIBS)

# Generar parser y header con bison
lang/parser.tab.c lang/parser.tab.h: lang/parser.y
	bison -d -o lang/parser.tab.c lang/parser.y

# Generar lexer con flex (depende del header generado por bison)
lang/lex.yy.c: lang/lexer.l lang/parser.tab.h
	flex -o lang/lex.yy.c lang/lexer.l

clean:
	rm -f $(TARGET)
	rm -f lang/parser.tab.c lang/parser.tab.h lang/lex.yy.c

.PHONY: all clean
