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

void translation(node* root, int depth);

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
                $$->stmt_type = STMT_DEF_VAR;
                $$->name = $2->name;
                $$->arr_size = 0;
            }
            | INT id_with_name OPEN_SQUARE_BRAC NUM CLOSE_SQUARE_BRAC SEMICOLON
            {
                $$ = new_node();
                $$->node_type = NODE_STMT;
                $$->stmt_type = STMT_DEF_VAR;
                $$->name = $2->name;
                $$->arr_size = (intval) << 2;
            }
            ;

decl_fun :  INT id_with_name OPEN_ROUND_BRAC param_list CLOSE_ROUND_BRAC SEMICOLON
            {
                $$ = new_node();
                $$->node_type = NODE_DECL_FUN;
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
                $$->children[0] = $7;
                $$->children[1] = $4;
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
                        $$->num_param = $2->num_param + 1;
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
                        $$ = $1;
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
                    $$->arr_size = (intval) << 2;
                }
                | INT id_with_name OPEN_SQUARE_BRAC CLOSE_SQUARE_BRAC
                {
                    $$ = new_node();
                    $$->node_type = NODE_STMT;
                    $$->stmt_type = STMT_DECL_VAR;
                    $$->name = $2->name;
                    $$->arr_size = -1;
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
                | NOT exp
                {
                    $$ = new_node();
                    $$->node_type = NODE_EXP;
                    $$->exp_type = EXP_UNIOP;
                    $$->op_type = OP_NOT;
                    $$->children[0] = $2;
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
                    $$->name = $1->name;
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
    //print_tree(root);
    translation(root, 0);
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
    if(root == NULL){
        return ;
    }
    if(root->name){
        printf("visit node with name: %s\n", root->name);
    }
    switch(root->node_type){
        case NODE_EXP:{
            printf("node type: expression\n");
            printf("expression type: %d\n", root->exp_type);
            for(int i = 0; i < 5; i += 1){
                print_tree(root->children[i]);
            }
            print_tree(root->next);
            break;
        }
        case NODE_STMT:{
            printf("node type: statement\n");
            printf("statement type: %d\n", root->stmt_type);
            for(int i = 0; i< 5; i += 1){
                print_tree(root->children[i]);
            }
            print_tree(root->next);
            break;
        }
        case NODE_DEF_FUN:{
            printf("node type: function definition\n");
            for(int i = 0; i< 5; i += 1){
                print_tree(root->children[i]);
            }
            print_tree(root->next);
            break;
        }
        case NODE_DECL_FUN:{
            printf("node type: function declaration\n");
            for(int i = 0; i< 5; i += 1){
                print_tree(root->children[i]);
            }
            print_tree(root->next);
            break;
        }
    }
}

int find_in_vtb(char* name){
    int pos_in_var_table = -1;
    for(int i = var_table_idx - 1; i >= 0; i -= 1){
        if(strcmp(var_table[i].name, name) == 0){
            pos_in_var_table = i;
            break;
        }
    }
    return pos_in_var_table;
}

void translation(node* root, int depth){
    if(root == NULL){
        return ;
    }
    if(root->node_type == NODE_DECL_FUN){
        var_table[var_table_idx].name = root->name;
        var_table[var_table_idx].num_param = root->num_param;
        var_table[var_table_idx].var_type = VAR_FUNC;
        var_table[var_table_idx].var_depth = depth;
        var_table_idx += 1;
        translation(root->next, 0);
    }
    if(root->node_type == NODE_DEF_FUN){
        /* TODO: what if a func being defined twice? */
        if(find_in_vtb(root->name) == -1){
            var_table[var_table_idx].name = root->name;
            var_table[var_table_idx].num_param = root->num_param;
            var_table[var_table_idx].var_type = VAR_FUNC;
            var_table_idx += 1;
        }
        int idx_without_param = var_table_idx;
        printf("f_%s[%d]\n", root->name, root->num_param);
        node* param = root->children[1];
        for(int i = 0; i < root->num_param; i += 1){
            var_table[var_table_idx].name = param->name;
            var_table[var_table_idx].var_idx_eeyore = i;
            var_table[var_table_idx].var_type = VAR_VARP;
            var_table[var_table_idx].var_depth = depth + 1; // sure?
            var_table_idx += 1;
            param = param->next;
        }
        translation(root->children[0], depth + 1);
        printf("end f_%s\n", root->name);
        var_table_idx = idx_without_param;
        translation(root->next, depth);
    }
    if(root->node_type == NODE_STMT){
        if(root->stmt_type == STMT_DEF_VAR){
            /* a simplest assumption: define a var no more than once*/
            int pos_in_var_table = find_in_vtb(root->name);
            if(pos_in_var_table != -1 && var_table[pos_in_var_table].var_depth == depth){
                printf("error: var %s already defined in the same scope, exit", root->name);
                exit(1);
            }
            root->var_idx_eeyore = varcnt_T;
            var_table[var_table_idx].name = root->name;
            var_table[var_table_idx].var_idx_eeyore = varcnt_T;
            var_table[var_table_idx].var_type = VAR_VART;
            if(root->arr_size == 0){
                printf("var T%d\n", varcnt_T);
            }else{
                printf("var %d T%d\n", root->arr_size, varcnt_T);
            }
            varcnt_T += 1;
            var_table_idx += 1;
        }
        if(root->stmt_type == STMT_IF){
            int idx_start = var_table_idx;
            translation(root->children[0], depth);
            root->lbl_idx_eeyore = lblcnt;
            lblcnt += 1;
            printf("if t%d == 0 goto l%d\n", root->children[0]->var_idx_eeyore, root->lbl_idx_eeyore);
            translation(root->children[1], depth + 1);
            printf("l%d:\n", root->lbl_idx_eeyore);
            var_table_idx = idx_start;
        }
        if(root->stmt_type == STMT_IFELSE){
            int idx_start = var_table_idx;
            translation(root->children[0], depth);
            root->lbl_idx_eeyore = lblcnt;
            lblcnt += 2;
            printf("if t%d == 0 goto l%d\n", root->children[0]->var_idx_eeyore, root->lbl_idx_eeyore);
            translation(root->children[1], depth + 1);
            var_table_idx = idx_start;
            idx_start = var_table_idx;
            printf("goto l%d\n", root->lbl_idx_eeyore + 1);
            printf("l%d:\n", root->lbl_idx_eeyore);
            translation(root->children[2], depth + 1);
            printf("l%d:\n", root->lbl_idx_eeyore + 1);
            var_table_idx = idx_start;
        }
        if(root->stmt_type == STMT_WHILE){
            int idx_start = var_table_idx;
            root->lbl_idx_eeyore = lblcnt;
            lblcnt += 2;
            printf("l%d:\n", root->lbl_idx_eeyore);
            translation(root->children[0], depth);
            printf("if t%d == 0 goto l%d\n", root->children[0]->var_idx_eeyore, root->lbl_idx_eeyore + 1);
            translation(root->children[1], depth + 1);
            printf("goto l%d\n", root->lbl_idx_eeyore);
            printf("l%d:\n", root->lbl_idx_eeyore + 1);
            var_table_idx = idx_start;
        }
        if(root->stmt_type == STMT_RETURN){
            translation(root->children[0], depth);
            printf("return t%d\n", root->children[0]->var_idx_eeyore);
        }
        if(root->stmt_type == STMT_IDASN){
            translation(root->children[0], depth);
            int pos_in_var_table = find_in_vtb(root->name);
            if(pos_in_var_table == -1){
                printf("error: var %s not defined, exit", root->name);
                exit(1);
            }
            if(var_table[pos_in_var_table].var_type == VAR_VARP){
                printf("p%d = t%d\n", var_table[pos_in_var_table].var_idx_eeyore, root->children[0]->var_idx_eeyore);
            }else{
                printf("T%d = t%d\n", var_table[pos_in_var_table].var_idx_eeyore, root->children[0]->var_idx_eeyore);
            }
        }
        if(root->stmt_type == STMT_ARRASN){
            translation(root->children[0], depth);
            translation(root->children[1], depth);
            int pos_in_var_table = find_in_vtb(root->name);
            if(pos_in_var_table == -1){
                printf("error: var %s not defined, exit", root->name);
                exit(1);
            }
            printf("t%d = 4 * t%d\n", root->children[0]->var_idx_eeyore, root->children[0]->var_idx_eeyore);
            if(var_table[pos_in_var_table].var_type == VAR_VARP){
                printf("p%d[t%d] = t%d\n", var_table[pos_in_var_table].var_idx_eeyore, root->children[0]->var_idx_eeyore, root->children[1]->var_idx_eeyore);
            }else{
                printf("T%d[t%d] = t%d\n", var_table[pos_in_var_table].var_idx_eeyore, root->children[0]->var_idx_eeyore, root->children[1]->var_idx_eeyore);
            }
        }
        if(root->stmt_type == STMT_NA){
            int idx_start = var_table_idx;
            translation(root->children[0], depth + 1);
            var_table_idx = idx_start;
        }
        translation(root->next, depth);
    }

    if(root->node_type == NODE_EXP){
        printf("var t%d\n", varcnt_t);
        root->var_idx_eeyore = varcnt_t;
        varcnt_t += 1;

        if(root->exp_type == EXP_BINOP){
            translation(root->children[0], depth);
            translation(root->children[1], depth);
            printf("t%d = t%d %s t%d\n", root->var_idx_eeyore, root->children[0]->var_idx_eeyore, op_table[root->op_type], root->children[1]->var_idx_eeyore);
        }
        if(root->exp_type == EXP_UNIOP){
            translation(root->children[0], depth);
            if(root->op_type == OP_NOT){
                printf("t%d = !t%d\n", root->var_idx_eeyore, root->children[0]->var_idx_eeyore);
            }else if(root->op_type == OP_UMINUS){
                printf("t%d = -t%d\n", root->var_idx_eeyore, root->children[0]->var_idx_eeyore);
            }
        }
        if(root->exp_type == EXP_NUM){
            printf("t%d = %d\n", root->var_idx_eeyore, root->val_num);
        }
        if(root->exp_type == EXP_ID){
            int pos_in_var_table = find_in_vtb(root->name);
            if(pos_in_var_table == -1){
                printf("error: var %s not defined, exit", root->name);
                exit(1);
            }
            if(var_table[pos_in_var_table].var_type == VAR_VARP){
                printf("t%d = p%d\n", root->var_idx_eeyore, var_table[pos_in_var_table].var_idx_eeyore);
            }else{
                printf("t%d = T%d\n", root->var_idx_eeyore, var_table[pos_in_var_table].var_idx_eeyore);
            }
        }
        if(root->exp_type == EXP_FUNCALL){
            int cnt_param = 0;
            node* tmp = root->children[0];
            while(tmp){
                cnt_param += 1;
                tmp = tmp->next;
            }
            int pos_in_var_table = find_in_vtb(root->name);
            if(pos_in_var_table == -1){
                printf("error: func %s not defined, exit", root->name);
                exit(1);
            }
            if(cnt_param != var_table[pos_in_var_table].num_param){
                printf("error: func %s parameter not match, exit", root->name);
                exit(1);
            }
            tmp = root->children[0];
            while(tmp){
                int pos_in_var_table = find_in_vtb(tmp->name);
                if(pos_in_var_table == -1){
                    printf("error: param %s not defined, exit", tmp->name);
                    exit(1);
                }
                if(var_table[pos_in_var_table].var_type == VAR_VARP){
                    printf("param p%d\n", var_table[pos_in_var_table].var_idx_eeyore);
                }else{
                    printf("param T%d\n", var_table[pos_in_var_table].var_idx_eeyore);
                }
                tmp = tmp->next;
            }
            printf("t%d = call f_%s\n", root->var_idx_eeyore, root->name);
        }
        if(root->exp_type == EXP_ARR){
            translation(root->children[0], depth);
            translation(root->children[1], depth);
            printf("t%d = 4 * t%d\n", root->children[1]->var_idx_eeyore, root->children[1]->var_idx_eeyore);
            printf("t%d = t%d[t%d]\n", root->var_idx_eeyore, root->children[0]->var_idx_eeyore, root->children[1]->var_idx_eeyore);
        }
    }
}
