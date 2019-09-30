

typedef struct node{
    struct node* children[5];
    struct node* next;
    int node_type;
    int stmt_type;
    int exp_type;
    int op_type;
    int val_type;
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
