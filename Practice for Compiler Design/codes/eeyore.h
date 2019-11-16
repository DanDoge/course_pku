#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum{NODE_DEF_VAR, NODE_DEF_FUN, NODE_EXP} Node_Type;
typedef enum{EXP_BINOP, EXP_UNIOP, EXP_ASN_RV, EXP_ARR_ASN, EXP_ASN_ARR, EXP_IF, EXP_GOTO, EXP_LBL, EXP_PARAM, EXP_FUNCALL, EXP_RETURN}  Exp_Type;
typedef enum{OP_EQ, OP_NEQ, OP_ASN, OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_MOD, OP_AND, OP_OR, OP_GE, OP_LE, OP_NOT, OP_ID, OP_NUM} Op_Type;
char op_table[16][4] = {"==", "!=", "=", "+", "-", "*", "/", "%", "&&", "||", ">", "<", "!"};

typedef struct Var_Table{
    char* name;
    int loc_in_stack;
    int size;
    int global;
    int num_param;
    int reg;
}var_table;

typedef struct Node{
    struct Node* children[5];
    struct Node* next;
    struct Node* parent;
    struct Node* prev;
    struct Node* last_stmt;
    struct Node* next_stmt[2];
    Node_Type node_type;
    Exp_Type exp_type;
    Op_Type op_type;
    char* name;
    int size;
    int val_num;
    int num_param;
    int stack_size;
    int use[128];
    int define[128];
    int live[128];
    var_table* local_var_table;
    int vtb_idx;
}node;

var_table global_var_table[10010];
int gvt_idx = 0;
