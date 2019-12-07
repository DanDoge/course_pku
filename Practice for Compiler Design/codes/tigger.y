%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define YYSTYPE char*

extern void yyerror(char* message); // defined in minic.l
extern char tokenString[10010];
extern int intval;
extern int yylineno;

char* cpy_string(char* s);

int local_stack_size;

typedef enum{OP_EQ, OP_NEQ, OP_ASN, OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_MOD, OP_AND, OP_OR, OP_GE, OP_LE, OP_NOT, OP_ID, OP_NUM} Op_Type;
char op_table[16][8] = {"==", "!=", "=", "add", "sub", "mul", "div", "rem", "&&", "||", "sgt", "slt", "!"};
Op_Type op_type;

%}

%token ADD SUB MUL DIV MOD AND OR NOT GE LE COLON OPEN_SQUARE_BRAC CLOSE_SQUARE_BRAC ASN NEQ EQ
%token IF GOTO CALL RETURN END LOAD LOADADDR STORE MALLOC
%token LABEL NUM ID REG FUNC

%%

ALL:    ALL single_line
        |
        ;

single_line:        id_with_name ASN NUM
                    {
                        printf(".global %s\n", $1);
                        printf(".section .sdata\n.align 2\n");
                        printf(".type %s, @object\n", $1);
                        printf(".size %s, 4\n", $1);
                        printf("%s:\n", $1);
                        printf(".word %d\n", intval);
                    }
                    | id_with_name ASN MALLOC NUM
                    {
                        printf(".comm %s,%d,4\n", $1, intval << 2);
                    }
                    | FUNC OPEN_SQUARE_BRAC NUM CLOSE_SQUARE_BRAC OPEN_SQUARE_BRAC NUM CLOSE_SQUARE_BRAC
                    {
                        local_stack_size = (intval / 4 + 1) << 4;
                        printf(".text\n");
                        printf(".align 2\n");
                        printf(".global %s\n", tokenString);
                        printf(".type %s,@function\n", tokenString);
                        printf("%s:\n", tokenString);
                        printf("add sp,sp,%d\n", -local_stack_size);
                        printf("sw ra, %d(sp)\n", local_stack_size - 4);
                    }
                    | END FUNC
                    {
                        printf(".size %s,.-%s\n", tokenString, tokenString);
                    }
                    | reg_with_name ASN reg_with_name BINOP reg_with_name
                    {
                        switch(op_type){
                            case OP_EQ:{
                                printf("xor %s,%s,%s\n", $1, $3, $5);
                                printf("seqz %s,%s\n", $1, $1);
                                break;
                            }
                            case OP_NEQ:{
                                printf("xor %s,%s,%s\n", $1, $3, $5);
                                printf("snez %s,%s\n", $1, $1);
                                break;
                            }
                            case OP_ASN:{
                                break;
                            }
                            case OP_ADD:
                            case OP_SUB:
                            case OP_MUL:
                            case OP_DIV:
                            case OP_MOD:
                            case OP_GE:
                            case OP_LE:{
                                printf("%s %s,%s,%s\n", op_table[op_type], $1, $3, $5);
                                break;
                            }
                            case OP_AND:{
                                printf("and %s,%s,%s\n", $1, $3, $5);
                                printf("snez %s,%s\n", $1, $1);
                                break;
                            }
                            case OP_OR:{
                                printf("or %s,%s,%s\n", $1, $3, $5);
                                printf("snez %s,%s\n", $1, $1);
                                break;
                            }
                        }
                    }
                    | reg_with_name ASN UNIOP reg_with_name
                    {
                        if(op_type == OP_SUB){
                            printf("sub %s,x0,%s\n", $1, $4);
                        }else{
                            printf("not %s,%s\n", $1, $4);
                        }
                    }
                    | reg_with_name ASN reg_with_name
                    {
                        printf("mv %s,%s\n", $1, $3);
                    }
                    | reg_with_name ASN NUM
                    {
                        printf("li %s,%d\n", $1, intval);
                    }
                    | reg_with_name OPEN_SQUARE_BRAC NUM CLOSE_SQUARE_BRAC ASN reg_with_name
                    {
                        printf("sw %s,-%d(%s)\n", $6, intval, $1);
                    }
                    | reg_with_name ASN reg_with_name OPEN_SQUARE_BRAC NUM CLOSE_SQUARE_BRAC
                    {
                        printf("lw %s,-%d(%s)\n", $1, intval, $3);
                    }
                    | IF reg_with_name EQ reg_with_name GOTO LABEL
                    {
                        printf("beq %s,%s,.%s\n", $2, $4, tokenString);
                    }
                    | GOTO LABEL
                    {
                        printf("j .%s\n", tokenString);
                    }
                    | LABEL COLON
                    {
                        printf(".%s:\n", tokenString);
                    }
                    | CALL FUNC
                    {
                        printf("call %s\n", tokenString);
                    }
                    | STORE reg_with_name NUM
                    {
                        printf("sw %s,%d(sp)\n", $2, intval << 2);
                    }
                    | LOAD NUM reg_with_name
                    {
                        printf("lw %s,%d(sp)\n", $3, intval << 2);
                    }
                    | LOAD id_with_name reg_with_name
                    {
                        printf("lui %s,%%hi(%s)\n", $3, $2);
                        printf("lw %s,%%lo(%s)(%s)\n", $3, $2, $3);
                    }
                    | LOADADDR NUM reg_with_name
                    {
                        printf("add %s,sp,%d\n", $3, intval << 2);
                    }
                    | LOADADDR id_with_name reg_with_name
                    {
                        printf("lui %s,%%hi(%s)\n", $3, $2);
                        printf("add %s,%s,%%lo(%s)\n", $3, $3, $2);
                    }
                    | RETURN
                    {
                        printf("lw ra,%d(sp)\n", local_stack_size - 4);
                        printf("add sp,sp,%d\n", local_stack_size);
                        printf("jr ra\n");
                    }
                    ;

reg_with_name:      REG
                    {
                        $$ = cpy_string(tokenString);
                    }
                    ;

id_with_name:       ID
                    {
                        $$ = cpy_string(tokenString);
                    }
                    ;

BINOP:              ADD
                    {
                        op_type = OP_ADD;
                    }
                    | SUB
                    {
                        op_type = OP_SUB;
                    }
                    | MUL
                    {
                        op_type = OP_MUL;
                    }
                    | DIV
                    {
                        op_type = OP_DIV;
                    }
                    | MOD
                    {
                        op_type = OP_MOD;
                    }
                    | AND
                    {
                        op_type = OP_AND;
                    }
                    | OR
                    {
                        op_type = OP_OR;
                    }
                    | GE
                    {
                        op_type = OP_GE;
                    }
                    | LE
                    {
                        op_type = OP_LE;
                    }
                    | NEQ
                    {
                        op_type = OP_NEQ;
                    }
                    | EQ
                    {
                        op_type = OP_EQ;
                    }
                    ;

UNIOP:              NOT
                    {
                        op_type = OP_NOT;
                    }
                    | SUB
                    {
                        op_type = OP_SUB;
                    }
                    ;

%%

int main(int argc, const char* argv[]){
    yyparse();
    return 0;
}

char* cpy_string(char* s){
    if(s == NULL){
        return NULL;
    }
    int length = strlen(s) + 1;
    char* tmp = malloc(length);
    if(tmp == NULL){
        printf("Out of mem when copying string: %s\n", s);
    }else{
        strcpy(tmp, s);
    }
    return tmp;
}
