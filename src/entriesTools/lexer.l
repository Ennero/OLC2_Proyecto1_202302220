%{
#include "parser.tab.h"
#include "error_reporter.h"
#include <stdio.h>
#include <string.h>

extern void yyerror(const char *s);
int yycolumn = 1;
char* yylasttext = NULL;

// Prototipo para la nueva función yywrap
int yywrap(void);

/*
   <<<<< CAMBIO CLAVE: Se mueve la macro YY_USER_ACTION aquí >>>>>
   La definición de la macro ahora está en el bloque de declaraciones C,
   fuera de la función yylex(), que es la ubicación correcta.
*/
#define YY_USER_ACTION                                   \
    yylloc.first_line   = yylineno;                      \
    yylloc.first_column = yycolumn;                      \
    yylloc.last_line    = yylineno;                      \
    yylloc.last_column  = yycolumn + yyleng - 1;         \
    yycolumn += yyleng;                                  \
    if (yylasttext) free(yylasttext);                    \
    yylasttext = strdup(yytext);

%}

/* Opciones de Flex */
%option yylineno nounput noinput warn

/* Condiciones de arranque para comentarios */
%x COMMENT

/* --- Definiciones de Expresiones Regulares --- */
digit             [0-9]
hex_digit         [0-9a-fA-F]
alpha             [a-zA-Z_]
alpha_num         ({alpha}|{digit})
identifier        {alpha}{alpha_num}*
integer           {digit}+
hex_integer       0[xX]{hex_digit}+
exponent          [eE][+-]?{digit}+
double_lit        ({digit}+\.{digit}*({exponent})?|\.{digit}+({exponent})?|{digit}+{exponent})
char_lit          \'([^'\\]|\\.)\'
string_lit        \"([^"\\]|\\.)*\"
line_comment      "//".*
block_comment     "/*"
white_space       [ \t\r]+
newline           \n

%%

{white_space}      { /* Ignorar */ }
{newline}          { yycolumn = 1; }
{line_comment}     { /* Ignorar */ }

{block_comment}    { BEGIN(COMMENT); }
<COMMENT>"*/"      { BEGIN(INITIAL); }
<COMMENT>.         { /* Ignorar */ }
<COMMENT>\n        { yycolumn = 1; }

"if"               return TOKEN_IF;
"else"             return TOKEN_ELSE;
"while"            return TOKEN_WHILE;
"for"              return TOKEN_FOR;
"switch"           return TOKEN_SWITCH;
"case"             return TOKEN_CASE;
"break"            return TOKEN_BREAK;
"continue"         return TOKEN_CONTINUE;
"return"           return TOKEN_RETURN;
"print"            return TOKEN_PRINT;
"main"             return TOKEN_MAIN;
"class"            return TOKEN_CLASS;
"int"              return TOKEN_DINT;
"double"           return TOKEN_DDOUBLE;
"boolean"          return TOKEN_DBOOLEAN;
"char"             return TOKEN_DCHAR;
"String"           return TOKEN_DSTRING;
"true"             { yylval.string = strdup(yytext); return TOKEN_TRUE; }
"false"            { yylval.string = strdup(yytext); return TOKEN_FALSE; }
{integer}          { yylval.string = strdup(yytext); return TOKEN_INTEGER; }
{hex_integer}      { yylval.string = strdup(yytext); return TOKEN_HEX_INTEGER; }
{double_lit}       { yylval.string = strdup(yytext); return TOKEN_DOUBLE_LIT; }
{char_lit}         { yylval.string = strdup(yytext); return TOKEN_CHAR_LITERAL; }
{string_lit}       { yylval.string = strdup(yytext); return TOKEN_STRING_LITERAL; }
{identifier}       { yylval.string = strdup(yytext); return TOKEN_IDENTIFIER; }
"=="               return TOKEN_IGUAL_IGUAL;
"!="               return TOKEN_DIFERENTE;
">="               return TOKEN_MAYOR_IGUAL;
"<="               return TOKEN_MENOR_IGUAL;
"&&"               return TOKEN_AND;
"||"               return TOKEN_OR;
"++"               return TOKEN_INCREMENTO;
"--"               return TOKEN_DECREMENTO;
[+\-*/%<>()\[\]{};,=.!] { return yytext[0]; }
.                  { add_error_to_report("Léxico", yytext, "Caracter ilegal no reconocido.", yylineno, yycolumn); }

%%

int yywrap(void) {
    if (YY_START == COMMENT) {
        add_error_to_report("Léxico", "EOF", "Fin de archivo inesperado dentro de un comentario en bloque.", yylineno, yycolumn);
        BEGIN(INITIAL);
    }
    return 1;
}
