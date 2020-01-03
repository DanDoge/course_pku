#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* notes on how to compile
bison -d fb3-1.y
flex -ofb3-1.lex.c fb3-1.l
cc -o $@ fb3-1.tab.c fb3-1.lex.c fb3-1funcs.c
*/

typedef enum{NODE_EXP, NODE_STMT, NODE_DEF_FUN, NODE_DECL_FUN} Node_Type;
typedef enum{STMT_NA, STMT_IF, STMT_IDASN, STMT_WHILE, STMT_IFELSE, STMT_ARRASN, STMT_RETURN, STMT_DECL_VAR, STMT_DEF_VAR, STMT_FOR, STMT_DOWHILE} Stmt_Type;
typedef enum{EXP_ID, EXP_ARR, EXP_NUM, EXP_BINOP, EXP_UNIOP, EXP_FUNCALL} Exp_Type;
typedef enum{VAL_INT, VAL_BOOL} Val_Type;
typedef enum{OP_GE, OP_LE, OP_EQ, OP_OR, OP_AND, OP_NOT, OP_DIV, OP_ADD, OP_SUB, OP_MOD, OP_MUL, OP_NEQ, OP_UMINUS} Op_Type;
char op_table[16][4] = {">", "<", "==", "||", "&&", "!", "/", "+", "-", "%", "*", "!=", "-"};
typedef enum{VAR_FUNC, VAR_VART, VAR_VARP} Var_Type;

typedef struct Node{
    struct Node* children[5];
    struct Node* next;
    Node_Type node_type;
    Stmt_Type stmt_type;
    Exp_Type exp_type;
    Op_Type op_type;
    Val_Type val_type;
    char *name;
    int var_idx_eeyore; // idx in eeyore or idx of label in eeyore
    //int lbl_idx_eeyore;
    //int num_param;
    //int arr_size;
    int val_num; // num: atoi(string); arr: arr_size; func: num_param;
}node;

node* root;

typedef struct Var_Table_Type{
    char* name;
    int var_idx_eeyore;
    int fun_num_param;
    Var_Type var_type;
    int var_depth;
}Var_Table;

Var_Table var_table[1024];

int var_table_idx = 0;

int varcnt_t = 0;
int varcnt_T = 0;
int varcnt_p = 0;
int lblcnt = 0;
