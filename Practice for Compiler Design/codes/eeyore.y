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

void test_la();

void linear_allocation();

void translate();

int allocate_reg(node* t, int idx);
void free_reg(node* t, int idx);

int get_result(int a, int b, Op_Type op);

node* root;

char reg_name[28][4] = {"x0", "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11",
                        "t0", "t1", "t2", "t3", "t4", "t5", "t6",
                        "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7"};
int reg_used[32] = {};
int param_idx;

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
                    $$->name = cpy_string($1->name);
                    $$->op_type = OP_ID;
                }
                | num_with_val
                {
                    $$ = $1;
                    $$->name = cpy_string($1->name);
                    $$->op_type = OP_NUM;
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
    test_la();
    linear_allocation();
    translate();
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
        tmp[i].in_stack = 0;
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

            }else{
                t_child->next_stmt[0] = t_child->next;
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
                            tmp_live[i] = tmp->next_stmt[0]->live[i] | tmp->next_stmt[1]->live[i];
                            tmp_live[i] = tmp_live[i] & (1 - tmp->define[i]);
                            tmp_live[i] = tmp_live[i] | tmp->use[i];
                            if(tmp_live[i] != tmp->live[i]){
                                stable = 0;
                            }
                            tmp->live[i] = tmp_live[i];
                        }
                    }else{
                        int tmp_live[128];
                        for(int i = 0; i < 128; i += 1){
                            tmp_live[i] = tmp->next_stmt[0]->live[i];
                            tmp_live[i] = tmp_live[i] & (1 - tmp->define[i]);
                            tmp_live[i] = tmp_live[i] | tmp->use[i];
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

void test_la(){
    node* t = root;
    while(t){
        if(t->node_type != NODE_DEF_FUN){
            t = t->next;
            continue;
        }
        node* t_child = t->children[0];
        int stmt_idx = 1;
        while(t_child){
            if(t_child->node_type == NODE_DEF_VAR){
                t_child = t_child->next;
                continue;
            }
            for(int i = 0; i < t->size; i += 1){
                if(t_child->live[i]){
                    t->local_var_table[i].last_stmt = stmt_idx;
                }
            }
            stmt_idx += 1;
            t_child = t_child->next;
        }

        t = t->next;
    }
}

int find_new_reg(int* reg){
    for(int i = 1; i < 11; i += 1){
        if(reg[i] == -1){
            return i;
        }
    }
    for(int i = 13; i < 20; i += 1){
        if(reg[i] == -1){
            return i;
        }
    }
    return -1;
}

int evict_one_var(int* reg, node* t, int stmt_idx){
    for(int i = 1; i < 11; i += 1){
        if(t->local_var_table[reg[i]].last_stmt + 1 < stmt_idx){
            reg[i] = -1;
            return i;
        }
    }
    for(int i = 13; i < 20; i += 1){
        if(t->local_var_table[reg[i]].last_stmt + 1 < stmt_idx){
            reg[i] = -1;
            return i;
        }
    }
    int largest_last_stmt = -1;
    int largest_idx = -1;
    for(int i = 1; i < 11; i += 1){
        if(t->local_var_table[reg[i]].last_stmt > largest_last_stmt){
            largest_last_stmt = t->local_var_table[reg[i]].last_stmt;
            largest_idx = i;
        }
    }
    for(int i = 13; i < 20; i += 1){
        if(t->local_var_table[reg[i]].last_stmt > largest_last_stmt){
            largest_last_stmt = t->local_var_table[reg[i]].last_stmt;
            largest_idx = i;
        }
    }
    //printf("evict %s from %s\n", t->local_var_table[reg[largest_idx]].name, reg_name[largest_idx]);
    t->local_var_table[reg[largest_idx]].reg = -1;
    reg[largest_idx] = -1;
    return largest_idx;
}

void linear_allocation(){
    node* t = root;
    while(t){
        if(t->node_type != NODE_DEF_FUN){
            t = t->next;
            continue;
        }
        node* t_child = t->children[0];

        int reg[32]; // [13, 20), [1, 11)
        for(int i = 0; i < 32; i += 1){
            reg[i] = -1;
        }
        int stmt_idx = 1;
        while(t_child){
            //printf("%d\n", stmt_idx);
            if(t_child->node_type == NODE_DEF_VAR){
                t_child = t_child->next;
                continue;
            }

            for(int i = 0; i < t->size; i += 1){
                if(t_child->live[i]){
                    //printf("%s ", t->local_var_table[i].name);
                    if(t->local_var_table[i].reg == -1){
                        continue;
                    }
                    if(t->local_var_table[i].reg == 0){
                        // allocate one
                        //printf("allocate reg for %s...", t->local_var_table[i].name);
                        int new_reg = find_new_reg(reg);
                        if(new_reg == -1){
                            new_reg = evict_one_var(reg, t, stmt_idx);
                        }
                        reg[new_reg] = i;
                        t->local_var_table[i].reg = new_reg;
                        //printf("done\n");
                    }
                    //printf("%s in %s ", t->local_var_table[i].name, reg_name[t->local_var_table[i].reg]);
                }
            }
            //printf("\n");
            stmt_idx += 1;
            t_child = t_child->next;
        }
        //printf("%s\n", t->name);
        for(int i = 0; i < t->size; i += 1){
            //printf("%s to %s, last_stmt %d\n", t->local_var_table[i].name, reg_name[t->local_var_table[i].reg], t->local_var_table[i].last_stmt);
            if(t->local_var_table[i].reg == 0){
                t->local_var_table[i].reg = 11;
            }
        }
        t = t->next;
    }
}


void translate(){
    node* t = root;
    while(t){
        if(t->node_type == NODE_DEF_VAR){
            int idx_in_globaltable = -1;
            for(int i = 0; i < gvt_idx; i += 1){
                if(strcmp(t->name, global_var_table[i].name) == 0){
                    idx_in_globaltable = i;
                    break;
                }
            }
            if(idx_in_globaltable != -1){
                if(t->size){
                    printf("v%d = malloc %d\n", idx_in_globaltable, t->size << 2);
                }else{
                    printf("v%d = 0\n", idx_in_globaltable);
                }
            }
            t = t->next;
            continue;
        }
        // t being a function
        printf("%s [%d] [%d]\n", t->name, t->num_param, t->stack_size);
        node* t_child = t->children[0];

        memset(reg_used, -1, sizeof(reg_used));

        for(int i = 0; i < t->size; i += 1){
            if(t->local_var_table[i].num_param){
                printf("store %s %d\n", reg_name[t->local_var_table[i].num_param + 19], t->local_var_table[i].loc_in_stack);
                t->local_var_table[i].in_stack = 1;
            }
        }

        while(t_child){
            if(t_child->node_type == NODE_DEF_VAR){
                t_child = t_child->next;
                continue;
            }
            switch(t_child->exp_type){
                case EXP_BINOP:{
                    //printf("op_type are: %d, %d, %d\n", t_child->children[1]->op_type, t_child->children[2]->op_type,  t_child->op_type);
                    int pos_in_vtb0 = get_vtb_idx(t_child, t_child->children[0]->name);
                    allocate_reg(t_child, pos_in_vtb0);
                    int reg0 = t_child->parent->local_var_table[pos_in_vtb0].reg;
                    if(t_child->children[1]->op_type == OP_ID && t_child->children[2]->op_type == OP_ID){
                        int pos_in_vtb1 = get_vtb_idx(t_child, t_child->children[1]->name);
                        int pos_in_vtb2 = get_vtb_idx(t_child, t_child->children[2]->name);
                        allocate_reg(t_child, pos_in_vtb1);
                        allocate_reg(t_child, pos_in_vtb2);
                        int reg1 = t_child->parent->local_var_table[pos_in_vtb1].reg;
                        int reg2 = t_child->parent->local_var_table[pos_in_vtb2].reg;

                        //printf("regs are %d, %d, %d\n", reg0, reg1, reg2);

                        printf("%s = %s %s %s\n", reg_name[reg0], reg_name[reg1], op_table[t_child->op_type], reg_name[reg2]);
                        free_reg(t_child, pos_in_vtb1);
                        free_reg(t_child, pos_in_vtb2);
                    }else if(t_child->children[1]->op_type == OP_ID && t_child->children[2]->op_type == OP_NUM){
                        int pos_in_vtb1 = get_vtb_idx(t_child, t_child->children[1]->name);
                        allocate_reg(t_child, pos_in_vtb1);
                        int reg1 = t_child->parent->local_var_table[pos_in_vtb1].reg;
                        printf("s11 = %d\n", t_child->children[2]->val_num);
                        printf("%s = %s %s s11\n", reg_name[reg0], reg_name[reg1], op_table[t_child->op_type]);
                        free_reg(t_child, pos_in_vtb1);
                    }else if(t_child->children[1]->op_type == OP_NUM && t_child->children[2]->op_type == OP_ID){
                        int pos_in_vtb2 = get_vtb_idx(t_child, t_child->children[2]->name);
                        allocate_reg(t_child, pos_in_vtb2);
                        int reg2 = t_child->parent->local_var_table[pos_in_vtb2].reg;
                        printf("s11 = %d\n", t_child->children[1]->val_num);
                        printf("%s = s11 %s %s\n", reg_name[reg0], op_table[t_child->op_type], reg_name[reg2]);
                        free_reg(t_child, pos_in_vtb2);
                    }else{
                        int res = get_result(t_child->children[1]->val_num, t_child->children[2]->val_num, t_child->op_type);
                        printf("%s = %d\n", reg_name[reg0], res);
                    }
                    free_reg(t_child, pos_in_vtb0);
                    break;
                }
                case EXP_UNIOP:{
                    int pos_in_vtb0 = get_vtb_idx(t_child, t_child->children[0]->name);
                    allocate_reg(t_child, pos_in_vtb0);
                    int reg0 = t_child->parent->local_var_table[pos_in_vtb0].reg;
                    if(t_child->children[1]->op_type == OP_ID){
                        int pos_in_vtb1 = get_vtb_idx(t_child, t_child->children[1]->name);
                        allocate_reg(t_child, pos_in_vtb1);
                        int reg1 = t_child->parent->local_var_table[pos_in_vtb1].reg;
                        printf("%s = %s %s\n", reg_name[reg0], op_table[t_child->op_type], reg_name[reg1]);
                        free_reg(t_child, pos_in_vtb1);
                    }else{
                        int tmp = 0;
                        if(t_child->op_type == OP_NOT){
                            tmp = !(t_child->children[1]->val_num);
                        }else{
                            tmp = -(t_child->children[1]->val_num);
                        }
                        printf("%s = %d\n", reg_name[reg0], tmp);
                    }
                    free_reg(t_child, pos_in_vtb0);
                    break;
                }
                case EXP_ASN_RV:{
                    int pos_in_vtb0 = get_vtb_idx(t_child, t_child->children[0]->name);
                    allocate_reg(t_child, pos_in_vtb0);
                    int reg0 = t_child->parent->local_var_table[pos_in_vtb0].reg;
                    if(t_child->children[1]->op_type == OP_NUM){
                        // num
                        printf("%s = %d\n", reg_name[reg0], t_child->children[1]->val_num);
                    }else{
                        // var
                        int pos_in_vtb1 = get_vtb_idx(t_child, t_child->children[1]->name);
                        allocate_reg(t_child, pos_in_vtb1);
                        int reg1 = t_child->parent->local_var_table[pos_in_vtb1].reg;
                        printf("%s = %s\n", reg_name[reg0], reg_name[reg1]);
                        free_reg(t_child, pos_in_vtb1);
                    }
                    free_reg(t_child, pos_in_vtb0);
                    break;
                }
                case EXP_ARR_ASN:{
                    // arr_name[idx] = rv ==> idx must be a var, rv could be a num
                    int pos_in_vtb0 = get_vtb_idx(t_child, t_child->children[0]->name);
                    allocate_reg(t_child, pos_in_vtb0);
                    int reg0 = t_child->parent->local_var_table[pos_in_vtb0].reg;

                    int pos_in_vtb1 = get_vtb_idx(t_child, t_child->children[1]->name);
                    allocate_reg(t_child, pos_in_vtb1);
                    int reg1 = t_child->parent->local_var_table[pos_in_vtb1].reg;
                    if(t_child->children[2]->op_type == OP_ID){
                        int pos_in_vtb2 = get_vtb_idx(t_child, t_child->children[2]->name);
                        allocate_reg(t_child, pos_in_vtb2);
                        int reg2 = t_child->parent->local_var_table[pos_in_vtb2].reg;

                        printf("s11 = %s + %s\n", reg_name[reg0], reg_name[reg1]);
                        printf("s11[0] = %s\n", reg_name[reg2]);

                        free_reg(t_child, pos_in_vtb2);
                    }else{
                        printf("s11 = %s + %s\n", reg_name[reg0], reg_name[reg1]);
                        printf("s10 = %d\n", t_child->children[2]->val_num);
                        printf("s11[0] = s10\n");
                    }
                    free_reg(t_child, pos_in_vtb1);
                    free_reg(t_child, pos_in_vtb0);
                    break;
                }
                case EXP_ASN_ARR:{
                    // var = arr[var], no number here
                    int pos_in_vtb0 = get_vtb_idx(t_child, t_child->children[0]->name);
                    allocate_reg(t_child, pos_in_vtb0);
                    int reg0 = t_child->parent->local_var_table[pos_in_vtb0].reg;
                    int pos_in_vtb1 = get_vtb_idx(t_child, t_child->children[1]->name);
                    allocate_reg(t_child, pos_in_vtb1);
                    int reg1 = t_child->parent->local_var_table[pos_in_vtb1].reg;
                    int pos_in_vtb2 = get_vtb_idx(t_child, t_child->children[2]->name);
                    allocate_reg(t_child, pos_in_vtb2);
                    int reg2 = t_child->parent->local_var_table[pos_in_vtb2].reg;
                    printf("s11 = %s + %s\n", reg_name[reg1], reg_name[reg2]);
                    printf("%s = s11[0]\n", reg_name[reg0]);

                    free_reg(t_child, pos_in_vtb0);
                    free_reg(t_child, pos_in_vtb1);
                    free_reg(t_child, pos_in_vtb2);
                    break;
                }
                case EXP_IF:{
                    // if var == 0 goto label
                    int pos_in_vtb0 = get_vtb_idx(t_child, t_child->children[0]->name);
                    allocate_reg(t_child, pos_in_vtb0);
                    int reg0 = t_child->parent->local_var_table[pos_in_vtb0].reg;
                    printf("if %s == x0 goto %s\n", reg_name[reg0], t_child->name);
                    free_reg(t_child, pos_in_vtb0);
                    break;
                }
                case EXP_GOTO:{
                    printf("goto %s\n", t_child->name);
                    break;
                }
                case EXP_LBL:{
                    printf("%s:\n", t_child->name);
                    break;
                }
                case EXP_PARAM:{
                    // param rv
                    if(param_idx == 0){
                        for(int i = 0; i < t_child->parent->size; i += 1){
                            if(t_child->parent->local_var_table[i].num_param){
                                int reg = t_child->parent->local_var_table[i].reg;
                                printf("store %s %d\n", reg_name[reg]
                                , t_child->parent->local_var_table[i].loc_in_stack);
                                t_child->parent->local_var_table[i].in_stack = 1;
                            }
                        }
                    }
                    if(t_child->children[0]->op_type == OP_ID){
                        int pos_in_vtb0 = get_vtb_idx(t_child, t_child->children[0]->name);
                        allocate_reg(t_child, pos_in_vtb0);
                        int reg0 = t_child->parent->local_var_table[pos_in_vtb0].reg;

                        printf("%s = %s\n", reg_name[param_idx + 20], reg_name[reg0]);
                        param_idx += 1;
                        free_reg(t_child, pos_in_vtb0);
                    }else{
                        printf("%s = %d\n", reg_name[param_idx + 20], t_child->children[0]->val_num);
                        param_idx += 1;
                    }
                    break;
                }
                case EXP_FUNCALL:{
                    param_idx = 0;

                    for(int i = 1; i < 32; i += 1){
                        if(reg_used[i] >= 0
                            && t_child->parent->local_var_table[reg_used[i]].size == 0
                            && t_child->parent->local_var_table[reg_used[i]].global == 0){
                            printf("store %s %d\n", reg_name[i], t_child->parent->local_var_table[reg_used[i]].loc_in_stack);
                        }
                    }

                    printf("call %s\n", t_child->name);

                    for(int i = 1; i < 32; i += 1){
                        if(reg_used[i] >= 0
                            && t_child->parent->local_var_table[reg_used[i]].size == 0
                            && t_child->parent->local_var_table[reg_used[i]].global == 0){
                            printf("load %d %s\n", t_child->parent->local_var_table[reg_used[i]].loc_in_stack, reg_name[i]);
                        }
                    }

                    int pos_in_vtb0 = get_vtb_idx(t_child, t_child->children[0]->name);
                    int return_val_vtb = 0;
                    allocate_reg(t_child, pos_in_vtb0);
                    int reg0 = t_child->parent->local_var_table[pos_in_vtb0].reg;
                    printf("%s = a0\n", reg_name[reg0]);

                    free_reg(t_child, pos_in_vtb0);

                    break;
                }
                case EXP_RETURN:{
                    if(t_child->children[0]->op_type == OP_ID){
                        int pos_in_vtb0 = get_vtb_idx(t_child, t_child->children[0]->name);
                        allocate_reg(t_child, pos_in_vtb0);
                        int reg0 = t_child->parent->local_var_table[pos_in_vtb0].reg;
                        printf("a0 = %s\n", reg_name[reg0]);
                        free_reg(t_child, pos_in_vtb0);
                    }else{
                        printf("a0 = %d\n", t_child->children[0]->val_num);
                    }
                    printf("return\n");
                    break;
                }
                default:{
                    printf("Error! No such exp_type %d!\n", t_child->exp_type);
                }
            }

            t_child = t_child->next;
        }

        printf("end %s\n", t->name);
        t = t->next;
    }
}

int get_result(int a, int b, Op_Type op){
    int res = 0;
    switch(op){
        case OP_EQ:{
            res = a == b;
            break;
        }
        case OP_NEQ:{
            res = a != b;
            break;
        }
        case OP_ASN:{
            res = a != b;
            break;
        }
        case OP_ADD:{
            res = a + b;
            break;
        }
        case OP_SUB:{
            res = a - b;
            break;
        }
        case OP_MUL:{
            res = a * b;
            break;
        }
        case OP_DIV:{
            res = a / b;
            break;
        }
        case OP_MOD:{
            res = a % b;
            break;
        }
        case OP_AND:{
            res = a && b;
            break;
        }
        case OP_OR:{
            res = a || b;
            break;
        }
        case OP_GE:{
            res = a > b;
            break;
        }
        case OP_LE:{
            res = a < b;
            break;
        }
    }
    return res;
}

/* [25, 28) */
int tmp_a_used[3];

void free_reg(node* t_child, int idx){
    if(t_child->next == NULL){
        return ;
    }
    if(t_child->parent->local_var_table[idx].size){
        return ;
    }
    if(t_child->parent->local_var_table[idx].reg != -1){
        if(t_child->parent->local_var_table[idx].reg >= 25){
            int reg = t_child->parent->local_var_table[idx].reg;
            reg_used[t_child->parent->local_var_table[idx].reg] = -1;
            tmp_a_used[t_child->parent->local_var_table[idx].reg - 25] = 0;
            if(t_child->parent->local_var_table[idx].global){
                printf("loadaddr v%d s11\n", idx);
                printf("s11[0] = %s\n", reg_name[reg]);
            }else{
                printf("store %s %d\n", reg_name[reg] , t_child->parent->local_var_table[idx].loc_in_stack);
            }
            t_child->parent->local_var_table[idx].reg = -1;
            return ;
        }else if(t_child->parent->local_var_table[idx].in_stack == 0 && t_child->parent->local_var_table[idx].global == 0){
            return ;
        }
    }
    if(t_child->parent->local_var_table[idx].global){
        printf("loadaddr v%d s11\n", idx);
        printf("s11[0] = %s\n", reg_name[t_child->parent->local_var_table[idx].reg]);
    }else{
        printf("store %s %d\n", reg_name[t_child->parent->local_var_table[idx].reg]
                              , t_child->parent->local_var_table[idx].loc_in_stack);
    }
    return ;
}

int allocate_reg(node* t_child, int idx){
    //printf("allocate var: %s idx: %d\n", t_child->parent->local_var_table[idx].name, idx);
    int reg = -1;
    if(t_child->parent->local_var_table[idx].reg != -1){
        if(t_child->parent->local_var_table[idx].in_stack == 0
            && t_child->parent->local_var_table[idx].global == 0
            && t_child->parent->local_var_table[idx].size == 0){
            reg_used[t_child->parent->local_var_table[idx].reg] = idx;
            return t_child->parent->local_var_table[idx].reg;
        }else{
            reg = t_child->parent->local_var_table[idx].reg;
        }
    }
    for(int i = 0; i < 3 && reg == -1; i += 1){
        if(tmp_a_used[i] == 0){
            reg = i + 25;
            t_child->parent->local_var_table[idx].reg = reg;
            tmp_a_used[i] = 1;
            break;
        }
    }
    if(t_child->parent->local_var_table[idx].size){
        if(t_child->parent->local_var_table[idx].global){
            printf("loadaddr v%d %s\n", idx, reg_name[reg]);
        }else if(t_child->parent->local_var_table[idx].num_param == 0){
            printf("loadaddr %d %s\n", t_child->parent->local_var_table[idx].loc_in_stack, reg_name[reg]);
        }
        return reg;
    }
    if(t_child->parent->local_var_table[idx].global == 0){
        printf("load %d %s\n", t_child->parent->local_var_table[idx].loc_in_stack, reg_name[reg]);
    }else{
        printf("load v%d %s\n", idx, reg_name[reg]);
    }
    if(t_child->parent->local_var_table[idx].reg != -1){
        reg_used[reg] = idx;
    }
    // t_child->parent->local_var_table[idx].in_stack = 0;
    return reg;
}
