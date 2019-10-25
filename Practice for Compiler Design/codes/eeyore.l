%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "eeyore.tab.h"

char tokenString[10010];
int intval;
%}

number      [0-9]+
character   [a-zA-Z]
ignore      [ \r\t\n]+

%option yylineno

%%

"var"       {return VAR;}
"if"        {return IF;}
"goto"      {return GOTO;}
"return"    {return RETURN;}
"call"      {return CALL;}
"end"       {return END;}
"param"     {return PARAM;}

"+"         {return ADD;}
"-"         {return SUB;}
"*"         {return MUL;}
"/"         {return DIV;}
"%"         {return MOD;}

"&&"        {return AND;}
"||"        {return OR;}
"!"         {return NOT;}

"="         {return ASN;}
"<"         {return LE;}
">"         {return GE;}
"=="        {return EQ;}
"!="        {return NEQ;}



":"         {return COLON;}
"["         {return OPEN_SQUARE_BRAC;}
"]"         {return CLOSE_SQUARE_BRAC;}

[l]number   {strcpy(tokenString, yytext); return LABEL;}
{number}    {intval = atoi(yytext); return NUM;}
character+number    {strcpy(tokenString, yytext); return ID;}
character+_number   {strcpy(tokenString, yytext); return FUNC;}
ignore      {}

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
