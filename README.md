# JavaLang: intérprete en C con IDE GTK y reportes AST

Intérprete de un lenguaje similar a Java, con lexer Flex, parser Bison, nodos AST, entornos y reportes de símbolos/errores. Incluye interfaz GTK y modo CLI para interpretar archivos .usl.

## Requisitos

Linux con GCC, make, Flex, Bison, pkg-config, GTK3, GtkSourceView 3 y Graphviz. La construcción se basa en el Makefile incluido.

## Ejecución

Los comandos parten de la raíz del repositorio, salvo donde se indica otra terminal.

```bash
# Debian/Ubuntu: instalar las dependencias necesarias
sudo apt install build-essential flex bison pkg-config libgtk-3-dev libgtksourceview-3.0-dev graphviz
make

# Ejecutar un ejemplo en CLI
./build/javalang --run calificacion/basicos.usl

# Abrir el IDE
./compile.sh
```

## Recorrido de demostración

Interpretar un archivo de calificacion/ o test/ y comparar consola, tabla de símbolos y errores. Desde el IDE también puede generarse el gráfico del AST.

## Organización

Carpetas de la entrega: [`ast-graph/`](ast-graph/), [`calificacion/`](calificacion/), [`docs/`](docs/), [`src/`](src/), [`test/`](test/).

## Documentación

- [informe_tecnico](docs/informe_tecnico.md)
- [manual_usuario](docs/manual_usuario.md)

## Alcance y límites

Esta entrega interpreta el AST; no se anuncia como generador/optimizador de código de tres direcciones. Para ensamblador ARM64, consultar la entrega OLC2_Proyecto2. Los binarios se generan con make.

La documentación anterior a la actualización se conserva en [README-anterior.md](README-anterior.md) como referencia histórica; para iniciar el proyecto, seguir esta guía.

## Contexto y licencia

Trabajo académico o de aprendizaje conservado en el portafolio de [Enner Mendizabal](https://github.com/Ennero). Los créditos de los trabajos en grupo se conservan en sus entregas y manuales.

Consultar [LICENSE](LICENSE) para la licencia de la raíz. Las dependencias y los componentes con avisos o licencias propios conservan sus condiciones.
