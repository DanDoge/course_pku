%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define YYSTYPE node *

extern void yyerror(char* message); // defined in minic.l
extern int yylineno;

char* cpy_string(char* s);

%}

%token VAR IF GOTO RETURN CALL END PARAM
%token ADD SUB MUL DIV MOD
%token AND OR NOT
%token ASN LE GE EQ NEQ
%token COLON OPEN_SQUARE_BRAC CLOSE_SQUARE_BRAC
%token NUM ID

%%

final:      goal
            {
                $$ = $1;
                root = $$;
            }
            ;

goal:       goal def_var
            {
                node* tmp = $1;
                if(tmp){
                    while(tmp->next){
                        tmp = tmp->next;
                    }
                    tmp->next = $2;
                    $$ = $1;
                }else{
                    $$ = $2;
                }
            }
            | goal def_func
            {
                node* tmp = $1;
                if(tmp){
                    while(tmp->next){
                        tmp = tmp->next;
                    }
                    tmp->next = $2;
                    $$ = $1;
                }else{
                    $$ = $2;
                }
            }
            |
            {
                $$ = NULL;
            }
            ;

def_var:    VAR id_with_name
            {
                $$ = new_node();
                $$->node_type = NODE_DEF_VAR;
                $$->name = $2->name;
                $$->size = 0;
            }
            | VAR num_with_val id_with_name
            {
                $$ = new_node();
                $$->node_type = NODE_DEF_VAR;
                $$->name = $3->name;
                $$->size = $2->val_num;
            }
            ;

def_func:   FUNC OPEN_ROUND_BRAC CLOSE_ROUND_BRAC inside_func END FUNC
            {
                $$ = new_node();
                $$->node_type = NODE_DEF_FUN;
                $$->children[0] = $4;
                $$->num_param = 0;
                $$->name = cpy_string(tokenString);
            }
            | FUNC OPEN_ROUND_BRAC num_with_val CLOSE_ROUND_BRAC inside_func END FUNC
            {
                $$ = new_node();
                $$->node_type = NODE_DEF_FUN;
                $$->children[0] = $5;
                $$->num_param = $3->val_num;
                $$->name = cpy_string(tokenString);
            }
            ;

inside_func:    inside_func def_var
                {
                    node* tmp = $1;
                    if(tmp){
                        while(tmp->next){
                            tmp = tmp->next;
                        }
                        tmp->next = $2;
                        $$ = $1;
                    }else{
                        $$ = $2;
                    }
                    $2->prev = tmp;
                }
                | inside_func exp
                {
                    node* tmp = $1;
                    if(tmp){
                        while(tmp->next){
                            tmp = tmp->next;
                        }
                        tmp->next = $2;
                        $$ = $1;
                    }else{
                        $$ = $2;
                    }
                    $2->prev = tmp;
                }
                |
                {
                    $$ = NULL;
                }
                ;

exp:            id_with_name ASN right_value BINOP right_value
                {
                    $$ = new_node();
                    $$->node_type = NODE_EXP;
                    $$->exp_type = EXP_BINOP;
                    $$->children[0] = $1;
                    $$->children[1] = $3;
                    $$->children[2] = $5;
                    $$->op_type = $4->op_type;
                }
                | id_with_name ASN UNIOP right_value
                {
                    $$ = new_node();
                    $$->node_type = NODE_EXP;
                    $$->exp_type = EXP_UNIOP;
                    $$->children[0] = $1;
                    $$->children[1] = $4;
                    $$->op_type = $3->op_type;
                }
                | id_with_name ASN right_value
                {
                    $$ = new_node();
                    $$->node_type = NODE_EXP;
                    $$->exp_type = EXP_ASN_RV;
                    $$->children[0] = $1;
                    $$->children[1] = $3;
                }
                | id_with_name OPEN_SQUARE_BRAC right_value CLOSE_SQUARE_BRAC ASN right_value
                {
                    $$ = new_node();
                    $$->node_type = NODE_EXP;
                    $$->exp_type = EXP_ARR_ASN;
                    $$->children[0] = $1;
                    $$->children[1] = $3;
                    $$->children[2] = $6;
                }
                | id_with_name ASN id_with_name OPEN_SQUARE_BRAC right_value CLOSE_SQUARE_BRAC
                {
                    $$ = new_node();
                    $$->node_type = NODE_EXP;
                    $$->exp_type = EXP_ASN_ARR;
                    $$->children[0] = $1;
                    $$->children[1] = $3;
                    $$->children[2] = $5;
                }
                | IF right_value BINOP right_value GOTO LABEL
                {
                    $$ = new_node();
                    $$->node_type = NODE_EXP;
                    $$->exp_type = EXP_IF;
                    $$->op_type = $3->op_type;
                    $$->name = cpy_string(tokenString); // label
                    $$->children[0] = $2;
                    $$->children[1] = $4;
                }
                | GOTO LABEL
                {
                    $$ = new_node();
                    $$->node_type = NODE_EXP;
                    $$->exp_type = EXP_GOTO;
                    $$->name = cpy_string(tokenString);
                }
                | LABEL COLON
                {
                    $$ = new_node();
                    $$->node_type = NODE_EXP;
                    $$->exp_type = EXP_LBL;
                    $$->name = cpy_string(tokenString);
                }
                | PARAM right_value
                {
                    $$ = new_node();
                    $$->node_type = NODE_EXP;
                    $$->exp_type = EXP_PARAM;
                    $$->children[0] = $2;
                }
                | id_with_name ASN CALL FUNC
                {
                    $$ = new_node();
                    $$->node_type = NODE_EXP;
                    $$->exp_type = EXP_FUNCALL;
                    $$->name = cpy_string(tokenString); // function name
                    $$->children[0] = $1;
                }
                | RETURN right_value
                {
                    $$ = new_node();
                    $$->node_type = NODE_EXP;
                    $$->exp_type = EXP_RETURN;
                    $$->children[0] = $2;
                }
                ;

right_value:    id_with_name
                {
                    $$ = $1;
                }
                | num_with_val
                {
                    $$ = $1;
                }
                ;

id_with_name:   ID
                {
                    $$ = new_node();
                    $$->node_type = NODE_ID;
                    $$->name = cpy_string(tokenString);
                }
                ;

num_with_val:   NUM
                {
                    $$ = new_node();
                    $$->node_type = NODE_NUM;
                    $$->val_num = intval;
                }

BINOP:          EQ
                {
                    $$ = new_node();
                    $$->op_type = OP_EQ
                }
                | NEQ
                {
                    $$ = new_node();
                    $$->op_type = OP_NEQ
                }
                | ASN
                {
                    $$ = new_node();
                    $$->op_type = OP_ASN
                }
                | ADD
                {
                    $$ = new_node();
                    $$->op_type = OP_ADD
                }
                | SUB
                {
                    $$ = new_node();
                    $$->op_type = OP_SUB
                }
                | MUL
                {
                    $$ = new_node();
                    $$->op_type = OP_MUL
                }
                | DIV
                {
                    $$ = new_node();
                    $$->op_type = OP_DIV
                }
                | MOD
                {
                    $$ = new_node();
                    $$->op_type = OP_MOD
                }
                | AND
                {
                    $$ = new_node();
                    $$->op_type = OP_AND
                }
                | OR
                {
                    $$ = new_node();
                    $$->op_type = OP_OR
                }
                | GE
                {
                    $$ = new_node();
                    $$->op_type = OP_GE
                }
                | LE
                {
                    $$ = new_node();
                    $$->op_type = OP_LE
                }
                ;

UNIOP:          NOT
                {
                    $$ = new_node();
                    $$->op_type = OP_NOT;
                }
                | SUB
                {
                    $$ = new_node();
                    $$->op_type = OP_SUB; // or OP_UMINUS?
                }
                ;

%%

int main(){
    yyparse();
    return 0;
}
