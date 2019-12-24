%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tigger.tab.h"

char tokenString[1010];
int intval;

%}

ignore      [ \t\n\r]+

%option yylineno

%%

"+"     {return ADD;}
"-"     {return SUB;}
"*"     {return MUL;}
"/"     {return DIV;}
"%"     {return MOD;}

"&&"    {return AND;}
"||"    {return OR;}
"!"     {return NOT;}

">"     {return GE;}
"<"     {return LE;}
":"     {return COLON;}
"["     {return OPEN_SQUARE_BRAC;}
"]"     {return CLOSE_SQUARE_BRAC;}

"="     {return ASN;}
"!="    {return NEQ;}
"=="    {return EQ;}

"if"        {return IF;}
"goto"      {return GOTO;}
"call"      {return CALL;}
"return"    {return RETURN;}
"end"       {return END;}
"load"      {return LOAD;}
"loadaddr"  {return LOADADDR;}
"store"     {return STORE;}
"malloc"    {return MALLOC;}

[l][0-9]+       {strcpy(tokenString, yytext); return LABEL;}
[0-9]+          {intval = atoi(yytext); return NUM;}
[v][0-9]+       {strcpy(tokenString, yytext); return ID;}
[s|t|a][0-9]+|[x][0]  {strcpy(tokenString, yytext); return REG;}
[a-zA-Z_][a-zA-Z0-9_]*   {strcpy(tokenString, yytext); return FUNC;}

{ignore}    {}

%%

/* define some functions, s.t. no need to link -lf */

int yywrap(){
    /* no need to parse more than one files */
    return 1;
}

void yyerror(char* msg){
    printf("Error occured in line %d when parsing, error type: %s  \n", yylineno, msg);
    exit(1);
}

/* define funcions ends */
