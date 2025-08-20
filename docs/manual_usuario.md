# Índice del Manual de Usuario - JavaLang Interpreter

- [Índice del Manual de Usuario - JavaLang Interpreter](#índice-del-manual-de-usuario---javalang-interpreter)
  - [Objetivos](#objetivos)
    - [General](#general)
    - [Específicos](#específicos)
  - [Introducción](#introducción)
  - [Requisitos del Sistema](#requisitos-del-sistema)
  - [Flujo de funcionalidades del programa](#flujo-de-funcionalidades-del-programa)

## Objetivos
### General
- Explicar el flujo de funcionamiento del intérprete de JavaLang, desde la edición del código hasta su ejecución y la visualización de reportes.

### Específicos
- Proporcionar una comprensión clara de la interacción entre los componentes del intérprete, como el análisis léxico, sintáctico, semántico y la ejecución del código. 
- Facilitar el uso de la herramienta.
- Detallar las funcionalidades del software, incluyendo la edición, ejecución y visualización de los resultados

## Introducción
El presente manual tiene como objetivo proporcionar los conocimientos necesarios para utilizar el intérprete de JavaLang de manera efectiva. En este se describe detalladamente las funcionalidades del software, con especial énfasis en su flujo de trabajo.
El programa es un intérprete funcional para un lenguaje académico llamado JavaLang, cuya sintaxis se inspira en Java. El objetivo principal es que los estudiantes puedan comprender e implementar los principales componentes de un compilador. El software incluye una interfaz gráfica que facilita la escritura, análisis y ejecución del código fuente.

El sistema permite al usuario crear, abrir, editar y guardar archivos con la extensión .usl. Una vez ejecutado, el intérprete es capaz de analizar, validar e interpretar el código. La salida del programa se muestra en un panel de consola, y además se generan reportes detallados del Árbol de Sintaxis Abstracta (AST), la tabla de símbolos y los errores detectados.

## Requisitos del Sistema
- Sistema Operativo: Compatible con los compiladores de C y bibliotecas de desarrollo requeridas.

- Compilador: Un compilador de C, como GCC.

- Herramientas de Generación: Flex para el análisis léxico y Bison para el análisis sintáctico.

- Herramientas de Compilación: Makefile.

- Biblioteca para la GUI: Gtk3.

- Hardware Mínimo: Se recomienda un procesador y memoria RAM suficientes para ejecutar un entorno de desarrollo y las herramientas mencionadas.


## Flujo de funcionalidades del programa