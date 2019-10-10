#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum{NODE_EXP, NODE_STMT, NODE_DEF_FUN, NODE_DECL_DUN} Node_Type;
typedef enum{STMT_NA, STMT_IF, STMT_IDASN, STMT_WHILE, STMT_IFELSE, STMT_ARRASN, STMT_RETURN, STMT_DECL_VAR} Stmt_Type;
typedef enum{EXP_ID, EXP_ARR, EXP_NUM, EXP_BINOP, EXP_UNIOP, EXP_FUNCALL} Exp_Type;
typedef enum{VAL_INT, VAL_BOOL} Val_Type;
typedef enum{OP_GE, OP_LE, OP_EQ, OP_OR, OP_AND, OP_DIV, OP_ADD, OP_SUB, OP_MOD, OP_MUL, OP_NEQ, OP_UMINUS} Op_Type;

typedef struct Node{
    struct Node* children[5];
    struct Node* next;
    Node_Type node_type;
    Stmt_Type stmt_type;
    Exp_Type exp_type;
    Op_Type op_type;
    Val_Type val_type;
    char *name;
    int start_val;
    int var_idx_eeyore;
    int num_param;
    int arr_size;
    int val_num;
}node;

node* root;

typedef struct var_table{
    char* name;
    int var_idx_eeyore;
    int type;
}var_table[1024];

int var_table_idx = 0;

int varcnt_t = 0;
int varcnt_p = 0;
int lblcnt = 0;
