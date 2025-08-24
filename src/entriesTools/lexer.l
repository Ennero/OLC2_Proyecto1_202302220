%{
#include "parser.tab.h"
#include <stdio.h>
#include <string.h>

// FUNCIÓN PARA PROCESAR ESCAPES
char* unescape_string(const char* s) {
    int len = strlen(s);
    // Asigna memoria
    char* dest = (char*)malloc(len + 1);
    if (!dest) {
        yyerror("Fallo de memoria");
        exit(1);
    }
    int j = 0; // Índice para la cadena de destino
    // Recorre la cadena de origen, ignorando las comillas iniciales y finales.
    for (int i = 1; i < len - 1; ++i) {
        if (s[i] == '\\') {
            i++; // Salta la barra para ver el siguiente carácter
            switch (s[i]) {
                case 'n': dest[j++] = '\n'; break;
                case 't': dest[j++] = '\t'; break;
                case 'r': dest[j++] = '\r'; break;
                case '\\': dest[j++] = '\\'; break;
                case '"': dest[j++] = '\"'; break;
                // Si es un escape desconocido copia el carácter (z)
                default: dest[j++] = s[i]; break; 
            }
        } else {
            dest[j++] = s[i];
        }
    }
    dest[j] = '\0'; // Asegura la terminación nula
    return dest;
}


char* unescape_char(const char* s) {
    char final_char;
    // El contenido del char está en s[1]
    if (s[1] == '\\') {
        // Es una secuencia de escape
        switch (s[2]) {
            case 'n': final_char = '\n'; break;
            case 't': final_char = '\t'; break;
            case 'r': final_char = '\r'; break;
            case '\\': final_char = '\\'; break;
            case '\'': final_char = '\''; break;
            case 'u': { 
                if (strlen(s) < 8) { // Debe ser '\uXXXX'
                    yyerror("Secuencia de escape Unicode mal formada");
                    final_char = '?';
                } else {
                    // Extraemos los 4 dígitos hexadecimales
                    char hex_code[5];
                    strncpy(hex_code, s + 3, 4);
                    hex_code[4] = '\0';
                    long value = strtol(hex_code, NULL, 16);
                    final_char = (char)value;
                }
                break;
            }
            default: final_char = s[2]; break; // Escapes desconocidos
        }
    } else {
        // Es un carácter normal
        final_char = s[1];
    }
    
    // El parser espera un char*, así que creamos una cadena de 1 carácter
    char* result = (char*)malloc(2);
    if (!result) {
        yyerror("Fallo de memoria");
        exit(1);
    }
    result[0] = final_char;
    result[1] = '\0';
    return result;
}




//void yyerror(char *message);

/* Contador de columna */
int yycolumn = 1;

/* Se ejecuta antes de cada accion, actualiza yylloc y columna */
#define YY_USER_ACTION                                   \
    yylloc.first_line   = yylineno;                      \
    yylloc.first_column = yycolumn;                      \
    yylloc.last_line    = yylineno;                      \
    yylloc.last_column  = yycolumn + yyleng - 1;         \
    yycolumn           += yyleng;
%}

/* Opciones de Flex 
 * yylineno:  mantenga el numero de la linea actual
    leida desde su entrada en la variable global yylineno.
 * noyywrap:  hace que el analizador no llame a yywrap() hasta el fin-de-fichero, 
    pero simplemente asume que no hay mas ficheros que analizar.
*/
%option noyywrap yylineno

/* Condiciones de arranque */
%x COMMENT2 COMMENTLINEA

/* definiciones */
white_space       [ \t]*
digit             [0-9]
alpha             [A-Za-z_]
alpha_num         ({alpha}|{digit})
hex_digit         [0-9A-F]
identifier        {alpha}{alpha_num}*
int               {digit}+
hex_integer       ${hex_digit}{hex_digit}*
exponent          e[+-]?{digit}+
i                 {int}
real              ({i}\.{i}?|{i}?\.{i}){exponent}?
string            \"([^\\\"\n]|\\.)*\"
bad_string        \"([^"\n]|\"\")+
char      \'([^\\']|\\.)\' 


%%
"//"                        BEGIN(COMMENTLINEA);
<COMMENTLINEA>[^\n\r]+ 
<COMMENTLINEA><<EOF>>       yyerror("EOF in comment");
<COMMENTLINEA>[\n\r]        BEGIN(INITIAL);

"/*"                        BEGIN(COMMENT2);
<COMMENT2>[^)*\n\r]+
<COMMENT2>[\n\r]            ++yylineno;
<COMMENT2><<EOF>>           yyerror("EOF in comment");
<COMMENT2>"*/"              BEGIN(INITIAL);
<COMMENT2>[*/]


int                         return(RW_INT);
float                       return(RW_FLOAT);
String                      return(RW_STRING);
boolean                     return(RW_BOOLEAN);
char                        return(RW_CHAR);

if                          return(RW_IF);
else                        return(RW_ELSE);
print                       return(RW_PRINT);


">"                         return(RW_MAYOR);
"<"                         return(RW_MENOR);
"!"                         return(RW_NEGACION);
"="                         return(RW_IGUAL);


{int}                { yylval.string = strdup(yytext); return(TK_INTEGER);}
{real}               { yylval.string = strdup(yytext); return(TK_REAL);}
{string}             { yylval.string = unescape_string(yytext); return(TK_STRING); }
{char}               { yylval.string = unescape_char(yytext); return(TK_CHAR); }
"true"               { yylval.string = strdup(yytext); return(TK_BOOLEAN); }
"false"              { yylval.string = strdup(yytext); return(TK_BOOLEAN); }
"null"               { yylval.string = strdup(yytext); return(TK_NULL); }



{bad_string}         yyerror("Unterminated string");

{identifier}         { yylval.string = strdup(yytext); return TK_IDENTIFIER; }

[*/+\-,^()\[\]\{\}\;]      return(yytext[0]);

{white_space}        /* do nothing */
[\n\r]               yylineno += 1;
.                    yyerror("Illegal input");

%%