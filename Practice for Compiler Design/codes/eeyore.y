%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "eeyore.h"

#define YYSTYPE node *

extern void yyerror(char* message); // defined in minic.l
extern char tokenString[10010];
extern int intval;
extern int yylineno;

char* cpy_string(char* s);
node* new_node();
int get_vtb_idx(node* root, char* name);
void get_var_table();
void get_next_stmt();
void live_analysis();

node* root;

%}

%token VAR IF GOTO RETURN CALL END PARAM
%token ADD SUB MUL DIV MOD
%token AND OR NOT
%token ASN LE GE EQ NEQ
%token COLON OPEN_SQUARE_BRAC CLOSE_SQUARE_BRAC
%token NUM ID
%token LABEL FUNC

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

def_func:   FUNC OPEN_SQUARE_BRAC CLOSE_SQUARE_BRAC inside_func END FUNC
            {
                $$ = new_node();
                $$->node_type = NODE_DEF_FUN;
                $$->children[0] = $4;
                $$->num_param = 0;
                $$->name = cpy_string(tokenString);
            }
            | FUNC OPEN_SQUARE_BRAC num_with_val CLOSE_SQUARE_BRAC inside_func END FUNC
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
                    //$$->node_type = NODE_ID;
                    $$->name = cpy_string(tokenString);
                    $$->op_type = OP_ID;
                }
                ;

num_with_val:   NUM
                {
                    $$ = new_node();
                    //$$->node_type = NODE_NUM;
                    $$->val_num = intval;
                    $$->op_type = OP_NUM;
                }

BINOP:          EQ
                {
                    $$ = new_node();
                    $$->op_type = OP_EQ;
                }
                | NEQ
                {
                    $$ = new_node();
                    $$->op_type = OP_NEQ;
                }
                | ASN
                {
                    $$ = new_node();
                    $$->op_type = OP_ASN;
                }
                | ADD
                {
                    $$ = new_node();
                    $$->op_type = OP_ADD;
                }
                | SUB
                {
                    $$ = new_node();
                    $$->op_type = OP_SUB;
                }
                | MUL
                {
                    $$ = new_node();
                    $$->op_type = OP_MUL;
                }
                | DIV
                {
                    $$ = new_node();
                    $$->op_type = OP_DIV;
                }
                | MOD
                {
                    $$ = new_node();
                    $$->op_type = OP_MOD;
                }
                | AND
                {
                    $$ = new_node();
                    $$->op_type = OP_AND;
                }
                | OR
                {
                    $$ = new_node();
                    $$->op_type = OP_OR;
                }
                | GE
                {
                    $$ = new_node();
                    $$->op_type = OP_GE;
                }
                | LE
                {
                    $$ = new_node();
                    $$->op_type = OP_LE;
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
    get_var_table();
    get_next_stmt();
    live_analysis();
    return 0;
}

node* new_node(){
    node* tmp = (node*)malloc(sizeof(node));
    for(int i = 0; i < 5; i += 1){
        tmp->children[i] = NULL;
    }
    tmp->name = NULL;
    tmp->next = NULL;
    tmp->parent = NULL;
    tmp->prev = NULL;
    tmp->last_stmt = NULL;
    tmp->next_stmt[0] = tmp->next_stmt[1] = NULL;
    memset(tmp->live, 0, sizeof(tmp->live));
    memset(tmp->define, 0, sizeof(tmp->define));
    memset(tmp->use, 0, sizeof(tmp->use));
    return tmp;
}

var_table* new_table(){
    var_table* tmp = (var_table* )malloc(10010 * sizeof(var_table));
    for(int i = 0; i < 10010; i += 1){
        tmp[i].name = NULL;
        tmp[i].loc_in_stack = 0;
        tmp[i].size = 0;
        tmp[i].global = 0;
        tmp[i].num_param = 0;
        tmp[i].reg = 0;
    }
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

void get_var_table(){
    node* t = root;
    while(t){
        if(t->node_type == NODE_DEF_VAR){
            global_var_table[gvt_idx].name = t->name;
            global_var_table[gvt_idx].size = t->size;
            global_var_table[gvt_idx].global = 1;
            gvt_idx += 1;
        }else if(t->node_type == NODE_DEF_FUN){
            t->local_var_table = new_table();
            for(int i = 0; i < gvt_idx; i += 1){
                t->local_var_table[t->size].name = global_var_table[i].name;
                t->local_var_table[t->size].size = global_var_table[i].size;
                t->local_var_table[t->size].global = 1;
                t->local_var_table[t->size].loc_in_stack = t->stack_size;
                t->size += 1;
                t->stack_size += 1;
            }
            for(int i = 0; i < t->num_param; i += 1){
                t->local_var_table[t->size].name = (char*)malloc(4 * sizeof(char));
                t->local_var_table[t->size].name[0] = 'p';
                t->local_var_table[t->size].name[1] = '0' + i;
                t->local_var_table[t->size].name[2] = '\0';
                t->local_var_table[t->size].size = 0;
                t->local_var_table[t->size].loc_in_stack = t->stack_size;
                t->local_var_table[t->size].num_param = i + 1; // for translation
                t->local_var_table[t->size].global = 0;
                t->size += 1;
                t->stack_size += 1;
            }
            if(t->children[0] == NULL){
                continue;
            }
            t->children[0]->parent = t;
            node* child = t->children[0];
            while(child){
                if(child->node_type == NODE_DEF_VAR){
                    child->parent->local_var_table[child->parent->size].name = child->name;
                    child->parent->local_var_table[child->parent->size].size = child->size;
                    child->parent->local_var_table[child->parent->size].global = 0;
                    child->parent->local_var_table[child->parent->size].loc_in_stack = child->parent->stack_size;
                    child->parent->size += 1;
                    if(child->size){
                        child->parent->stack_size += child->size;
                    }else{
                        child->parent->stack_size += 1;
                    }
                }

                if(child->next){
                    child->next->parent = child->parent;
                }else{
                    child->parent->last_stmt = child;
                }
                child = child->next;
            }
        }
        t = t->next;
    }
}

void get_next_stmt(){
    node* t = root;
    while(t){
        if(t->node_type != NODE_DEF_FUN){
            t = t->next;
            continue;
        }

        node* t_child = t->children[0];
        if(t_child == NULL){
            continue;
        }

        while(t_child){
            if(t_child->node_type == NODE_EXP){
                if(t_child->exp_type == EXP_IF){
                    node* node_label = t->children[0];
                    while(node_label){
                        if(node_label->node_type == NODE_EXP && node_label->exp_type == EXP_LBL
                            && strcmp(t_child->name, node_label->name) == 0){
                                t_child->next_stmt[0] = t_child->next;
                                t_child->next_stmt[1] = node_label;
                                break;
                            }
                        node_label = node_label->next;
                    }
                }

                else if(t_child->exp_type == EXP_GOTO){
                    node* node_label = t->children[0];
                    while(node_label){
                        if(node_label->node_type == NODE_EXP && node_label->exp_type == EXP_LBL
                            && strcmp(t_child->name, node_label->name) == 0){
                                t_child->next_stmt[0] = node_label;
                                break;
                            }
                        node_label = node_label->next;
                    }
                }

                else{
                    t_child->next_stmt[0] = t_child->next;
                }

            }
            t_child = t_child->next;
        }

        t = t->next;
    }
}

void live_analysis(){
    node* t = root;
    while(t){
        if(t->node_type != NODE_DEF_FUN){
            t = t->next;
            continue;
        }

        node* t_child = t->children[0];
        while(t_child){
            if(t_child->node_type == NODE_DEF_VAR){
                t_child = t_child->next;
                continue;
            }
            // exp_type EXP_BINOP EXP_UNIOP EXP_ASN_RV EXP_ARR_ASN EXP_ASN_ARR
            // EXP_IF EXP_GOTO EXP_LBL EXP_PARAM EXP_FUNCALL EXP_RETURN
            switch(t_child->exp_type){
                case EXP_BINOP:{
                    t_child->define[get_vtb_idx(t_child, t_child->children[0]->name)] = 1;
                    if(t_child->children[1]->op_type == OP_ID){ // being a variable
                        t_child->use[get_vtb_idx(t_child, t_child->children[1]->name)] = 1;
                    }
                    if(t_child->children[2]->op_type == OP_ID){ // being a variable
                        t_child->use[get_vtb_idx(t_child, t_child->children[2]->name)] = 1;
                    }
                    break;
                }
                case EXP_UNIOP:{
                    // same as EXP_ASN_RV
                }
                case EXP_ASN_RV:{
                    t_child->define[get_vtb_idx(t_child, t_child->children[0]->name)] = 1;
                    if(t_child->children[1]->op_type == OP_ID){ // being a variable
                        t_child->use[get_vtb_idx(t_child, t_child->children[1]->name)] = 1;
                    }
                    break;
                }
                case EXP_ARR_ASN:{
                    t_child->use[get_vtb_idx(t_child, t_child->children[0]->name)] = 1;
                    if(t_child->children[1]->op_type == OP_ID){ // being a variable
                        t_child->use[get_vtb_idx(t_child, t_child->children[1]->name)] = 1;
                    }
                    if(t_child->children[2]->op_type == OP_ID){ // being a variable
                        t_child->use[get_vtb_idx(t_child, t_child->children[2]->name)] = 1;
                    }
                    break;
                }
                case EXP_ASN_ARR:{
                    t_child->define[get_vtb_idx(t_child, t_child->children[0]->name)] = 1;
                    t_child->use[get_vtb_idx(t_child, t_child->children[1]->name)] = 1; // arrname cannot be a constant
                    if(t_child->children[2]->op_type == OP_ID){ // being a variable
                        t_child->use[get_vtb_idx(t_child, t_child->children[2]->name)] = 1;
                    }
                    break;
                }
                case EXP_IF:{
                    if(t_child->children[0]->op_type == OP_ID){ // being a variable
                        t_child->use[get_vtb_idx(t_child, t_child->children[0]->name)] = 1;
                    }
                    if(t_child->children[1]->op_type == OP_ID){ // being a variable
                        t_child->use[get_vtb_idx(t_child, t_child->children[1]->name)] = 1;
                    }
                    break;
                }
                case EXP_GOTO:{

                }
                case EXP_LBL:{
                    break;
                }
                case EXP_PARAM:{
                    if(t_child->children[0]->op_type == OP_ID){ // being a variable
                        t_child->use[get_vtb_idx(t_child, t_child->children[0]->name)] = 1;
                    }
                    break;
                }
                case EXP_FUNCALL:{
                    t_child->define[get_vtb_idx(t_child, t_child->children[0]->name)] = 1;
                    break;
                }
                case EXP_RETURN:{
                    if(t_child->children[0]->op_type == OP_ID){ // being a variable
                        t_child->use[get_vtb_idx(t_child, t_child->children[0]->name)] = 1;
                    }
                    break;
                }
                default:{
                    printf("Error! No such exp_type %d!\n", t_child->exp_type);
                }
            }
            t_child = t_child->next;
        }
        t = t->next;
    }

    t = root;
    while(t){
        if(t->node_type != NODE_DEF_FUN){
            t = t->next;
            continue;
        }

        int stable = 0;
        while(!stable){
            node* tmp = t->last_stmt;
            stable = 1;
            while(tmp){
                if(tmp->next_stmt[0] == NULL){
                    for(int i = 0; i < 128; i += 1){
                        tmp->live[i] = tmp->use[i];
                    }
                }else{
                    // bool algebra
                    if(tmp->node_type == NODE_EXP && tmp->exp_type == EXP_IF){
                        int tmp_live[128];
                        for(int i = 0; i < 128; i += 1){
                            tmp_live[i] = (tmp->next_stmt[0]->live[i] + tmp->next_stmt[1]->live[i]) != 0;
                            tmp_live[i] = tmp_live[i] * (1 - tmp->define[i]);
                            tmp_live[i] = (tmp_live[i] + tmp->use[i]) != 0;
                            if(tmp_live[i] != tmp->live[i]){
                                stable = 0;
                            }
                            tmp->live[i] = tmp_live[i];
                        }
                    }else{
                        int tmp_live[128];
                        for(int i = 0; i < 128; i += 1){
                            tmp_live[i] = (tmp->next_stmt[0]->live[i]) != 0;
                            tmp_live[i] = tmp_live[i] * (1 - tmp->define[i]);
                            tmp_live[i] = (tmp_live[i] + tmp->use[i]) != 0;
                            if(tmp_live[i] != tmp->live[i]){
                                stable = 0;
                            }
                            tmp->live[i] = tmp_live[i];
                        }
                    }
                }
                tmp = tmp->prev;
            }
        }
        t = t->next;
    }
}

int get_vtb_idx(node* root, char* name){
    node* parent = root->parent;
    for(int i = 0; i < parent->size; i += 1){
        if(strcmp(parent->local_var_table[i].name, name) == 0){
            return i;
        }
    }
    return -1;
}
