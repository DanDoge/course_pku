%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "minic.h"

extern void yyerror(char* message); // defined in minic.l

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

%%

final : decl main_func
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

decl :  decl decl_var
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
                $$->type = DEF_FUNC;
                $$->children[0] = $6;
                $$->num_param = 0;
            }
            ;

decl_var :  INT id_with_name SEMICOLON
            {
                $$ = new_node();
                $$->node_type = NODE_STMT;
                $$->stmt_type = STMT_DECL_VAR;
                $$->name = $2->name;
            }
            | INT id_with_name OPEN_SQUARE_BRAC num_with_val CLOSE_SQUARE_BRAC SEMICOLON
            {
                $$ = new_node();
                $$->node_type = NODE_STMT;
                $$->stmt_type = STMT_DECL_VAR;
                $$->name = $2->name;
                $$->arr_size = ($4->val_num) << 2;
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
                    $$->name = yylval.tokenname; // may need to deep copy a string
                }
                ;

num_with_val :  NUM
                {
                    $$ = new_node();
                    $$->name = yylval.intval;
                }
                ;

param_list :    decl_var_wo_colon param_left
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

param_left :    param_left COLON decl_var_wo_colon
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

decl_var_wo_colon : INT id_with_name
                    {
                        $$ = new_node();
                        $$->node_type = NODE_STMT;
                        $$->stmt_type = STMT_DECL_VAR;
                        $$->name = $2->name;
                    }
                    | INT id_with_name OPEN_SQUARE_BRAC num_with_val CLOSE_SQUARE_BRAC
                    {
                        $$ = new_node();
                        $$->node_type = NODE_STMT;
                        $$->stmt_type = STMT_DECL_VAR;
                        $$->name = $2->name;
                        $$->arr_size = ($4->val_num) << 2;
                    }
                    ;
