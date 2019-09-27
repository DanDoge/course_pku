#include <iostream>
using namespace std;

struct node{
    int l; // included
    int r; // not included
    int num_nodes;
}seg_tree[400010];

int stat[100010];
int res[100010];

void build_tree(int pos, int left, int right){
    seg_tree[pos].l = left;
    seg_tree[pos].r = right;
    seg_tree[pos].num_nodes = right - left;

    if(right - left <= 1){
        return ;
    }

    int mid = (left + right) / 2;
    build_tree(pos << 1, left, mid);
    build_tree((pos << 1) + 1, mid, right);
    return ;
}

int query(int val, int pos){
    seg_tree[val].num_nodes -= 1;
    if(seg_tree[val].l >= seg_tree[val].r - 1){
        return seg_tree[val].r - 1;
    }else if(seg_tree[val << 1].num_nodes >= pos){
        return query(val << 1, pos);
    }else{
        return query((val << 1) + 1, pos - seg_tree[val << 1].num_nodes);
    }
}

int main(int argc, char const *argv[]) {
    ios::sync_with_stdio(false);
    int size = 0;
    cin >> size;
    for(int i = 2; i <= size; i += 1){
        cin >> stat[i];
    }
    build_tree(1, 1, size + 1);
    for(int i = size; i >= 1; i -= 1){
        res[i] = query(1, stat[i] + 1);
    }
    for(int i = 1; i <= size; i += 1){
        cout << res[i] << endl;
    }
    return 0;
}
