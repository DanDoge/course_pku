%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "minic.h"

#define YYSTYPE node *

extern void yyerror(char* message); // defined in minic.l

node* new_node();

char* cpy_string(char* s);

void print_tree(node* root);

extern char tokenString[10010];
extern int intval;

%}

%token IF ELSE WHILE INT MAIN RETURN
%token ADD SUB MUL DIV MOD
%token AND OR NOT
%token ASN LE GE EQ NEQ
%token SEMICOLON COLON OPEN_CURLY_BRAC OPEN_ROUND_BRAC OPEN_SQUARE_BRAC CLOSE_CURLY_BRAC CLOSE_ROUND_BRAC CLOSE_SQUARE_BRAC
%token NUM ID

%left ASN
%left OR AND
%left EQ NEQ
%left GE LE
%left ADD SUB
%left MUL DIV MOD
%right NOT
%nonassoc UMINUS

%%

goal : decl main_func
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
            root = $$;
        }
        ;

decl :  decl def_var
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
        | decl decl_fun
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
        | decl def_fun
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

main_func : INT MAIN OPEN_ROUND_BRAC CLOSE_ROUND_BRAC OPEN_CURLY_BRAC stmt_list CLOSE_CURLY_BRAC
            {
                $$ = new_node();
                $$->name = "main";
                $$->node_type = NODE_DEF_FUN;
                $$->children[0] = $6;
                $$->num_param = 0;
            }
            ;

def_var :  INT id_with_name SEMICOLON
            {
                $$ = new_node();
                $$->node_type = NODE_STMT;
                $$->stmt_type = STMT_DECL_VAR;
                $$->name = $2->name;
            }
            | INT id_with_name OPEN_SQUARE_BRAC NUM CLOSE_SQUARE_BRAC SEMICOLON
            {
                $$ = new_node();
                $$->node_type = NODE_STMT;
                $$->stmt_type = STMT_DECL_VAR;
                $$->name = $2->name;
                $$->arr_size = (intval) << 2;
            }
            ;

decl_fun :  INT id_with_name OPEN_ROUND_BRAC param_list CLOSE_ROUND_BRAC SEMICOLON
            {
                $$ = new_node();
                $$->node_type = NODE_DECL_DUN;
                $$->name = $2->name;
                $$->children[0] = $4;
                if($4){
                    $$->num_param = $4->num_param;
                }else{
                    $$->num_param = 0;
                }
            }
            ;

def_fun :  INT id_with_name OPEN_ROUND_BRAC param_list CLOSE_ROUND_BRAC OPEN_CURLY_BRAC stmt_list CLOSE_CURLY_BRAC
            {
                $$ = new_node();
                $$->name = $2->name;
                $$->node_type = NODE_DEF_FUN;
                $$->children[0] = $4;
                $$->children[1] = $7;
                if($4){
                    $$->num_param = $4->num_param;
                }else{
                    $$->num_param = 0;
                }
            }
            ;

id_with_name :  ID
                {
                    $$ = new_node();
                    $$->name = cpy_string(tokenString); // may need to deep copy a string
                }
                ;

param_list :    decl_var param_left
                {
                    $$ = $1;
                    $$->next = $2;
                    if($2){
                        $$->num_param = $$->num_param + 1;
                    }else{
                        $$->num_param = 1;
                    }
                }
                |
                {
                    $$ = NULL;
                }
                ;

param_left :    param_left COLON decl_var
                {
                    node* tmp = $1;
                    if(tmp){
                        while(tmp->next){
                            tmp = tmp->next;
                        }
                        tmp->next = $3;
                        $$ = tmp;
                        $$->num_param = $1->num_param + 1;
                    }else{
                        $$ = $3;
                        $$->num_param = 1;
                    }
                }
                |
                {
                    $$ = NULL;
                }
                ;

decl_var :      INT id_with_name
                {
                    $$ = new_node();
                    $$->node_type = NODE_STMT;
                    $$->stmt_type = STMT_DECL_VAR;
                    $$->name = $2->name;
                }
                | INT id_with_name OPEN_SQUARE_BRAC NUM CLOSE_SQUARE_BRAC
                {
                    $$ = new_node();
                    $$->node_type = NODE_STMT;
                    $$->stmt_type = STMT_DECL_VAR;
                    $$->name = $2->name;
                    $$->arr_size = ($4->val_num) << 2;
                }
                ;

stmt_list:      stmt_list stmt
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

stmt :           OPEN_CURLY_BRAC stmt_list CLOSE_CURLY_BRAC
                {
                    $$ = new_node();
                    $$->node_type = NODE_STMT;
                    $$->stmt_type = STMT_NA;
                    $$->children[0] = $2;
                }
                | IF OPEN_ROUND_BRAC exp CLOSE_ROUND_BRAC stmt
                {
                    $$ = new_node();
                    $$->node_type = NODE_STMT;
                    $$->stmt_type = STMT_IF;
                    $$->children[0] = $3;
                    $$->children[1] = $5;
                }
                | IF OPEN_ROUND_BRAC exp CLOSE_ROUND_BRAC stmt ELSE stmt
                {
                    $$ = new_node();
                    $$->node_type = NODE_STMT;
                    $$->stmt_type = STMT_IFELSE;
                    $$->children[0] = $3;
                    $$->children[1] = $5;
                    $$->children[2] = $7;
                }
                | WHILE OPEN_ROUND_BRAC exp CLOSE_ROUND_BRAC stmt
                {
                    $$ = new_node();
                    $$->node_type = NODE_STMT;
                    $$->stmt_type = STMT_WHILE;
                    $$->children[0] = $3;
                    $$->children[1] = $5;
                }
                | id_with_name ASN exp SEMICOLON
                {
                    $$ = new_node();
                    $$->node_type = NODE_STMT;
                    $$->stmt_type = STMT_IDASN;
                    $$->name = $1->name;
                    $$->children[0] = $3;
                }
                | id_with_name OPEN_SQUARE_BRAC exp CLOSE_SQUARE_BRAC ASN exp SEMICOLON
                {
                    $$ = new_node();
                    $$->node_type = NODE_STMT;
                    $$->stmt_type = STMT_ARRASN;
                    $$->name = $1->name;
                    $$->children[0] = $3;
                    $$->children[1] = $6;
                }
                | def_var
                {
                    $$ = $1;
                }
                | RETURN exp SEMICOLON
                {
                    $$ = new_node();
                    $$->node_type = NODE_STMT;
                    $$->stmt_type = STMT_RETURN;
                    $$->children[0] = $2;
                }

exp :           exp ADD exp
                {
                    $$ = new_node();
                    $$->node_type = NODE_EXP;
                    $$->exp_type = EXP_BINOP;
                    $$->op_type = OP_ADD;
                    $$->val_type = VAL_INT;
                    $$->children[0] = $1;
                    $$->children[1] = $3;
                }
                | exp SUB exp
                {
                    $$ = new_node();
                    $$->node_type = NODE_EXP;
                    $$->exp_type = EXP_BINOP;
                    $$->op_type = OP_SUB;
                    $$->val_type = VAL_INT;
                    $$->children[0] = $1;
                    $$->children[1] = $3;
                }
                | exp MUL exp
                {
                    $$ = new_node();
                    $$->node_type = NODE_EXP;
                    $$->exp_type = EXP_BINOP;
                    $$->op_type = OP_MUL;
                    $$->val_type = VAL_INT;
                    $$->children[0] = $1;
                    $$->children[1] = $3;
                }
                | exp DIV exp
                {
                    $$ = new_node();
                    $$->node_type = NODE_EXP;
                    $$->exp_type = EXP_BINOP;
                    $$->op_type = OP_DIV;
                    $$->val_type = VAL_INT;
                    $$->children[0] = $1;
                    $$->children[1] = $3;
                }
                | exp MOD exp
                {
                    $$ = new_node();
                    $$->node_type = NODE_EXP;
                    $$->exp_type = EXP_BINOP;
                    $$->op_type = OP_MOD;
                    $$->val_type = VAL_INT;
                    $$->children[0] = $1;
                    $$->children[1] = $3;
                }
                | exp AND exp
                {
                    $$ = new_node();
                    $$->node_type = NODE_EXP;
                    $$->exp_type = EXP_BINOP;
                    $$->op_type = OP_AND;
                    $$->val_type = VAL_BOOL;
                    $$->children[0] = $1;
                    $$->children[1] = $3;
                }
                | exp OR exp
                {
                    $$ = new_node();
                    $$->node_type = NODE_EXP;
                    $$->exp_type = EXP_BINOP;
                    $$->op_type = OP_OR;
                    $$->val_type = VAL_BOOL;
                    $$->children[0] = $1;
                    $$->children[1] = $3;
                }
                | exp LE exp
                {
                    $$ = new_node();
                    $$->node_type = NODE_EXP;
                    $$->exp_type = EXP_BINOP;
                    $$->op_type = OP_LE;
                    $$->val_type = VAL_BOOL;
                    $$->children[0] = $1;
                    $$->children[1] = $3;
                }
                | exp GE exp
                {
                    $$ = new_node();
                    $$->node_type = NODE_EXP;
                    $$->exp_type = EXP_BINOP;
                    $$->op_type = OP_GE;
                    $$->val_type = VAL_BOOL;
                    $$->children[0] = $1;
                    $$->children[1] = $3;
                }
                | exp EQ exp
                {
                    $$ = new_node();
                    $$->node_type = NODE_EXP;
                    $$->exp_type = EXP_BINOP;
                    $$->op_type = OP_EQ;
                    $$->val_type = VAL_BOOL;
                    $$->children[0] = $1;
                    $$->children[1] = $3;
                }
                | exp NEQ exp
                {
                    $$ = new_node();
                    $$->node_type = NODE_EXP;
                    $$->exp_type = EXP_BINOP;
                    $$->op_type = OP_NEQ;
                    $$->val_type = VAL_BOOL;
                    $$->children[0] = $1;
                    $$->children[1] = $3;
                }
                | exp OPEN_SQUARE_BRAC exp CLOSE_SQUARE_BRAC
                {
                    $$ = new_node();
                    $$->node_type = NODE_EXP;
                    $$->exp_type = EXP_ARR;
                    $$->children[0] = $1;
                    $$->children[1] = $3;
                }
                | NUM
                {
                    $$ = new_node();
                    $$->node_type = NODE_EXP;
                    $$->exp_type = EXP_NUM;
                    $$->val_num = intval;
                }
                | id_with_name
                {
                    $$ = new_node();
                    $$->node_type = NODE_EXP;
                    $$->exp_type = EXP_ID;
                    $$->name = $1->name;
                }
                | SUB exp   %prec UMINUS
                {
                    $$ = new_node();
                    $$->node_type = NODE_EXP;
                    $$->exp_type = EXP_UNIOP;
                    $$->op_type = OP_UMINUS;
                    $$->children[0] = $2;
                }
                | id_with_name OPEN_ROUND_BRAC id_list CLOSE_ROUND_BRAC
                {
                    $$ = new_node();
                    $$->node_type = NODE_EXP;
                    $$->exp_type = EXP_FUNCALL;
                    $$->children[0] = $3;
                }
                | OPEN_ROUND_BRAC exp CLOSE_ROUND_BRAC
                {
                    $$ = $2;
                }
                ;

id_list :       id_with_name id_left
                {
                    $$ = new_node();
                    $$->name = $1->name;
                    $$->next = $2;
                }
                |
                {
                    $$ = NULL;
                }
                ;

id_left :       id_left COLON id_with_name
                {
                    node* tmp = $1;
                    if(tmp){
                        while(tmp->next){
                            tmp = tmp->next;
                        }
                        tmp->next = $3;
                        $$ = $1;
                    }else{
                        $$ = $3;
                    }
                }
                |
                {
                    $$ = NULL;
                }
                ;

%%

int main(int argc, const char* argv[]){
    yyparse();
    print_tree(root);
    return 0;
}

node* new_node(){
    node* tmp = (node*)malloc(sizeof(node));
    for(int i = 0; i < 5; i += 1){
        tmp->children[i] = NULL;
    }
    tmp->name = NULL;
    tmp->next = NULL;
    return tmp;
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

void print_tree(node* root){
    if(name){
        printf("visit node with name: %s\n", name);
    }
    switch(root->node_type){
        case NODE_EXP:{
            printf("node type: expression\n");
            printf("expression type: %d\n", root->stmt_type);

            break;
        }
        case NODE_STMT:{
            printf("node type: statement\n");
            break;
        }
        case NODE_DEF_FUN:{
            printf("node type: function definition\n");
            break;
        }
        case NODE_DECL_DUN:{
            printf("node type: function declaration\n");
            break;
        }
    }
}
