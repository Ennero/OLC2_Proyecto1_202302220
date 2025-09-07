# Informe Técnico - JavaLang Interpreter

## Índice
- [Informe Técnico - JavaLang Interpreter](#informe-técnico---javalang-interpreter)
  - [Índice](#índice)
  - [Objetivos](#objetivos)
    - [General](#general)
    - [Específicos](#específicos)
  - [Introducción](#introducción)
  - [Gramática Formal de Javalang](#gramática-formal-de-javalang)
  - [Diseño de Módulos](#diseño-de-módulos)
  - [Decisiones de Diseño](#decisiones-de-diseño)
  - [Desafíos Enfrentados](#desafíos-enfrentados)

## Objetivos

### General
Proporcionar una documentación técnica exhaustiva y detallada del Intérprete de JavaLang, que sirva como una referencia completa para entender su arquitectura, diseño, implementación y funcionamiento.


### Específicos
·	**Describir la Arquitectura del Sistema:** Detallar las herramientas, tecnologías y el entorno de desarrollo utilizados para la construcción del intérprete.
·	**Explicar el Proceso de Análisis Léxico:** Documentar la implementación del analizador léxico, incluyendo la definición de los tokens, el manejo de expresiones regulares y la generación de la tabla de tokens.
·	**Detallar el Análisis Sintáctico:** Explicar el diseño de la gramática del lenguaje JavaLang, la construcción del Árbol de Sintaxis Abstracta (AST) y la implementación del analizador sintáctico para la validación de la estructura del código fuente.
·	**Documentar la Gestión Semántica:** Describir la creación y administración de la tabla de símbolos, así como los mecanismos implementados para la detección y el reporte de errores semánticos.


## Introducción
Este manual tiene como objetivo proporcionar los conocimientos necesarios para utilizar el intérprete de JavaLang de manera efectiva. Aquí se describen detalladamente las funcionalidades del software, con especial énfasis en su flujo de trabajo.

El programa es un intérprete funcional para un lenguaje académico llamado JavaLang, cuya sintaxis se inspira en Java. El objetivo principal es que los estudiantes puedan comprender e implementar los principales componentes de un compilador. El software incluye una interfaz gráfica que facilita la escritura, análisis y ejecución del código fuente.

El sistema permite al usuario crear, abrir, editar y guardar archivos con la extensión `.usl`. Una vez ejecutado, el intérprete es capaz de analizar, validar e interpretar el código. La salida del programa se muestra en un panel de consola, y además se generan reportes detallados del Árbol de Sintaxis Abstracta (AST), la tabla de símbolos y los errores detectados.

## Gramática Formal de Javalang
Antes de comenzar a utilizar el intérprete de JavaLang, asegúrese de que su sistema cumple con los siguientes requisitos:
- **Sistema Operativo**: Compatible con los compiladores de C y bibliotecas de desarrollo requeridas.
- **Compilador**: Un compilador de C, como GCC, para compilación y ejecución del código.
- **Herramientas de Generación**: Flex para el análisis léxico y Bison para el análisis sintáctico.
- **Herramientas de Compilación**: Makefile para gestionar la compilación del proyecto.
- **Biblioteca para la GUI**: Gtk3 para la interfaz gráfica del usuario.
- **Hardware Mínimo**: Se recomienda un procesador y memoria RAM suficientes para ejecutar un entorno de desarrollo y las herramientas mencionadas.

## Diseño de Módulos




## Decisiones de Diseño



## Desafíos Enfrentados


