SRC     := src
BUILD   := build

CC      := gcc
BISON   := bison
FLEX    := flex

# Configuración para GTK3
GTK_CFLAGS := $(shell pkg-config --cflags gtk+-3.0)
GTK_LIBS   := $(shell pkg-config --libs gtk+-3.0)

# Flags de compilación
CFLAGS  := -I$(SRC) -I$(BUILD) -Wall -Wextra -std=c99 -g $(GTK_CFLAGS)
LDFLAGS := $(GTK_LIBS) -lm

# Comandos
RM      := rm -rf $(BUILD)
MKDIR   := mkdir -p $(BUILD)

.DEFAULT_GOAL := all

# Buscar todos los .c en src (excluyendo main.c si existe por separado)
SRC_FILES := $(shell find $(SRC) -name '*.c' -not -name 'main.c')

# Archivo principal (asumiendo que está en la raíz del proyecto)
MAIN_FILE := src/main.c

# Archivos generados por Bison y Flex
BISON_C := $(BUILD)/parser.tab.c
BISON_H := $(BUILD)/parser.tab.h
LEX_C   := $(BUILD)/lex.yy.c

# Objetos finales
OBJ_SRC := $(patsubst $(SRC)/%.c,$(BUILD)/%.o,$(SRC_FILES))
OBJ_MAIN := $(BUILD)/main.o
OBJ_GEN := $(BUILD)/parser.tab.o $(BUILD)/lex.yy.o
OBJ_ALL := $(OBJ_SRC) $(OBJ_MAIN) $(OBJ_GEN)

# Nombre del ejecutable
TARGET := javalang-interpreter

all: $(BUILD)/$(TARGET)

$(BUILD):
	$(MKDIR)

# Verificar dependencias de GTK3
check-deps:
	@echo "Verificando dependencias..."
	@pkg-config --exists gtk+-3.0 || (echo "Error: GTK+ 3.0 no encontrado. Instalar con: sudo apt-get install libgtk-3-dev" && exit 1)
	@which $(BISON) > /dev/null || (echo "Error: Bison no encontrado. Instalar con: sudo apt-get install bison" && exit 1)
	@which $(FLEX) > /dev/null || (echo "Error: Flex no encontrado. Instalar con: sudo apt-get install flex" && exit 1)
	@echo "Todas las dependencias están disponibles."

# Generar parser con Bison
$(BISON_C) $(BISON_H) $(BUILD)/parser.output: $(SRC)/entriesTools/parser.y | $(BUILD)
	@echo "Generando parser con Bison..."
	$(BISON) -d -v --locations --defines=$(BISON_H) -o $(BISON_C) $<

# Generar lexer con Flex
$(LEX_C): $(SRC)/entriesTools/lexer.l $(BISON_H) | $(BUILD)
	@echo "Generando lexer con Flex..."
	$(FLEX) -o $@ $<

# Compilar main.c
$(BUILD)/main.o: $(MAIN_FILE) | $(BUILD)
	@echo "Compilando $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Compilar archivos .c de src a .o
$(BUILD)/%.o: $(SRC)/%.c | $(BUILD)
	@mkdir -p $(dir $@)
	@echo "Compilando $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Compilar parser generado por Bison
$(BUILD)/parser.tab.o: $(BISON_C) $(BISON_H)
	@echo "Compilando parser..."
	$(CC) $(CFLAGS) -Wno-unused-function -Wno-unused-variable -c $(BISON_C) -o $@

# Compilar lexer generado por Flex
$(BUILD)/lex.yy.o: $(LEX_C)
	@echo "Compilando lexer..."
	$(CC) $(CFLAGS) -Wno-unused-function -Wno-unused-variable -c $(LEX_C) -o $@

# Enlazar todo para crear el ejecutable final
$(BUILD)/$(TARGET): $(OBJ_ALL)
	@echo "Enlazando ejecutable..."
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Compilación exitosa: $@"

# Ejecutar el programa
run: $(BUILD)/$(TARGET)
	@echo "Ejecutando JavaLang Interpreter..."
	./$(BUILD)/$(TARGET)

# Limpiar archivos de compilación
clean:
	@echo "Limpiando archivos de compilación..."
	$(RM)

# Limpiar y recompilar todo
rebuild: clean all

# Instalar dependencias en Ubuntu/Debian
install-deps:
	@echo "Instalando dependencias para Ubuntu/Debian..."
	sudo apt-get update
	sudo apt-get install -y build-essential libgtk-3-dev bison flex pkg-config

# Instalar dependencias en Fedora/CentOS/RHEL
install-deps-fedora:
	@echo "Instalando dependencias para Fedora/CentOS/RHEL..."
	sudo dnf install -y gcc gtk3-devel bison flex pkgconfig make

# Instalar dependencias en Arch Linux
install-deps-arch:
	@echo "Instalando dependencias para Arch Linux..."
	sudo pacman -S --needed base-devel gtk3 bison flex pkgconf

# Mostrar información de depuración
debug: CFLAGS += -DDEBUG -O0
debug: $(BUILD)/$(TARGET)
	@echo "Compilado en modo debug"

# Compilación optimizada para release
release: CFLAGS += -O2 -DNDEBUG
release: $(BUILD)/$(TARGET)
	@echo "Compilado en modo release"

# Mostrar información del proyecto
info:
	@echo "=== JavaLang Interpreter Build Info ==="
	@echo "Directorio fuente: $(SRC)"
	@echo "Directorio build: $(BUILD)"
	@echo "Compilador: $(CC)"
	@echo "GTK CFLAGS: $(GTK_CFLAGS)"
	@echo "GTK LIBS: $(GTK_LIBS)"
	@echo "Archivos fuente encontrados: $(words $(SRC_FILES))"
	@echo "Ejecutable objetivo: $(BUILD)/$(TARGET)"

# Verificar sintaxis sin compilar completamente
syntax-check:
	@echo "Verificando sintaxis de archivos fuente..."
	@for file in $(SRC_FILES) $(MAIN_FILE); do \
		echo "Verificando $$file..."; \
		$(CC) $(CFLAGS) -fsyntax-only $$file || exit 1; \
	done
	@echo "Verificación de sintaxis completada."

# Crear un archivo de prueba de ejemplo
create-test:
	@echo "Creando archivo de prueba ejemplo.usl..."
	@mkdir -p tests
	@echo '// Archivo de prueba para JavaLang' > tests/ejemplo.usl
	@echo '// ejemplo.usl' >> tests/ejemplo.usl
	@echo '' >> tests/ejemplo.usl
	@echo 'public static void main() {' >> tests/ejemplo.usl
	@echo '    int x = 10;' >> tests/ejemplo.usl
	@echo '    float y = 3.14f;' >> tests/ejemplo.usl
	@echo '    String mensaje = "Hola JavaLang";' >> tests/ejemplo.usl
	@echo '    ' >> tests/ejemplo.usl
	@echo '    System.out.println(mensaje);' >> tests/ejemplo.usl
	@echo '    System.out.println("Valor de x: " + x);' >> tests/ejemplo.usl
	@echo '    ' >> tests/ejemplo.usl
	@echo '    if (x > 5) {' >> tests/ejemplo.usl
	@echo '        System.out.println("x es mayor que 5");' >> tests/ejemplo.usl
	@echo '        ' >> tests/ejemplo.usl
	@echo '        for (int i = 0; i < 3; i++) {' >> tests/ejemplo.usl
	@echo '            System.out.println("Iteración: " + i);' >> tests/ejemplo.usl
	@echo '        }' >> tests/ejemplo.usl
	@echo '    }' >> tests/ejemplo.usl
	@echo '    ' >> tests/ejemplo.usl
	@echo '    // Ejemplo de arreglo' >> tests/ejemplo.usl
	@echo '    int[] numeros = {1, 2, 3, 4, 5};' >> tests/ejemplo.usl
	@echo '    System.out.println("Longitud del arreglo: " + numeros.length);' >> tests/ejemplo.usl
	@echo '}' >> tests/ejemplo.usl
	@echo "Archivo de prueba creado en tests/ejemplo.usl"

.PHONY: all clean rebuild run check-deps install-deps install-deps-fedora install-deps-arch debug release info syntax-check create-test