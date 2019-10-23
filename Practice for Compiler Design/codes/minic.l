%option yylineno

%{
#include <stdio.h>
#include <string.h>
#include"minic.tab.h" // bison on my machine generates "minic.tab.h" automatically

char tokenString[10010];
int intval;
%}

/* regex definition begins here */

number      [0-9]+
identifier  [a-zA-Z_][a-zA-Z0-9_]*
ignore      [ \r\t\n]+
comments    "/*"([^*]|"*"+[^"/"])*"*"+"/"
comment     "//"[^\n]*

/* regex definition ends here */

%%


"if"        {return IF;}
"else"      {return ELSE;}
"while"     {return WHILE;}
"int"       {return INT;}
"main"      {return MAIN;}
"return"    {return RETURN;}


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



";"         {return SEMICOLON;}
","         {return COLON;}
"{"         {return OPEN_CURLY_BRAC;}
"}"         {return CLOSE_CURLY_BRAC;}
"["         {return OPEN_SQUARE_BRAC;}
"]"         {return CLOSE_SQUARE_BRAC;}
"("         {return OPEN_ROUND_BRAC;}
")"         {return CLOSE_ROUND_BRAC;}



{number}        {intval = atoi(yytext); return NUM;}
{identifier}    {strcpy(tokenString, yytext); return ID;}
{comments}      {}
{comment}       {}
{ignore}        {}


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
