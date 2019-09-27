%option yylineno

%{
#include <stdio.h>
#include <string>
#include"minic.tab.h" // bison on my machine generates "minic.tab.h" automatically
%}

/* regex definition begins here */

number      [0-9]+
identifier  [a-zA-Z_][a-zA-Z0-9_]*
ignore      [ \t\n]+
comments    "/*"([^*]|"*"+[^"/"])*"*"+"/" /* for multi-line comments */
comment     "//"[^\n]*                    /* for single line comments */

/* regex definition ends here */

%%

/* reserved words begins here */

"if"        {return IF;}
"else"      {return ELSE;}
"while"     {return WHILE;}
"int"       {return INT;}
"main"      {return MAIN;}
"return"    {return RETURN;}

/* reserved words ends here */

/* operators begins here */

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

/* operators ends here */

/* punctuations begins here */

";"         {return SEMICOLON;}
","         {return COLON;}
"{"         {return OPEN_CURLY_BRAC;}
"}"         {return CLOSE_CURLY_BRAC;}
"["         {return OPEN_SQUARE_BRAC;}
"]"         {return CLOSE_SQUARE_BRAC;}
"("         {return OPEN_ROUND_BRAC;}
")"         {return CLOSE_ROUND_BRAC;}

/* punctuations ends here */

/* lexical analysis begins here */

{number}        {yylval.intval = atoi(yytext); return NUM;}
{identifier}    {strcpy(yylval.tokenname, yytext); return ID;}
{comments}      {}
{comment}       {}
{ignore}        {}

/* lexical analysis ends here */

%%

/* define some functions, s.t. no need to link -lf */

int yywrap(){
    /* no need to parse more than one file */
    return 1;
}

void yyerror(char* msg){
    printf("Error occured in %d line when parsing  %s  \n", yylineno, msg);
    exit(0);
}

/* define funcions ends */
