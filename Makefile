SRC     := src
BUILD   := build

CC      := gcc
BISON   := bison
FLEX    := flex

# Se separan las flags de compilación (cflags) y las de enlazado (libs) de GTK3
GTK_CFLAGS := $(shell pkg-config --cflags gtk+-3.0)
GTK_LIBS   := $(shell pkg-config --libs gtk+-3.0)

# Se añaden las flags de compilación de GTK a la variable CFLAGS principal
CFLAGS  := -I$(SRC) -Wall -Wextra $(GTK_CFLAGS)
RM      := rm -rf
MKDIR   := mkdir -p

.DEFAULT_GOAL := all

# Buscar todos los .c en src. Esto encontrará automáticamente output_buffer.c
SRC_FILES := $(shell find $(SRC) -name '*.c')

# Archivos generados por Bison y Flex
BISON_C := $(BUILD)/parser.tab.c
BISON_H := $(BUILD)/parser.tab.h
LEX_C   := $(BUILD)/lex.yy.c

# Objetos finales
OBJ_SRC := $(patsubst $(SRC)/%.c,$(BUILD)/%.o,$(SRC_FILES))
OBJ_GEN := $(BUILD)/parser.tab.o $(BUILD)/lex.yy.o
OBJ_ALL := $(OBJ_SRC) $(OBJ_GEN)

all: $(BUILD)/calc

$(BUILD):
	$(MKDIR) $(BUILD)

# Generar parser
$(BISON_C) $(BISON_H): $(SRC)/entriesTools/parser.y | $(BUILD)
	$(BISON) -d -v --locations -o $(BISON_C) $<

# Generar lexer
$(LEX_C): $(SRC)/entriesTools/lexer.l $(BISON_H) | $(BUILD)
	$(FLEX) -o $@ $<

# Compilar .c de src a .o
# Esta regla ahora usará las CFLAGS que incluyen las rutas de GTK
$(BUILD)/%.o: $(SRC)/%.c | $(BUILD)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Compilar .c generados por Bison/Flex a .o
$(BUILD)/parser.tab.o: $(BISON_C) $(BISON_H)
	$(CC) $(CFLAGS) -c $(BISON_C) -o $@

$(BUILD)/lex.yy.o: $(LEX_C)
	$(CC) $(CFLAGS) -c $(LEX_C) -o $@

# Enlazar todo, usando ahora solo las librerías de GTK
$(BUILD)/calc: $(OBJ_ALL)
	$(CC) $^ -o $@ $(GTK_LIBS)

clean:
	$(RM) $(BUILD)

.PHONY: all clean