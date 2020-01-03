#include <iostream>
using namespace std;

struct node{
    int l, r, len;
}seg_tree[100002 << 2];

int line[100002];
int res[100002];

void build(int s, int e, int pos){
    seg_tree[pos].l = s;
    seg_tree[pos].r = e;
    seg_tree[pos].len = e - s;
    if(s >= e - 1){
        return ;
    }
    int mid = (s + e) >> 1;
    build(s, mid, pos * 2);
    build(mid, e, pos * 2 + 1);
    return ;
}

int update(int k, int pos){
	//cout << k << " " << pos << endl;
    seg_tree[pos].len -= 1;
    if(seg_tree[pos].l >= seg_tree[pos].r - 1){
        return seg_tree[pos].r;
    }
    if(seg_tree[pos * 2].len >= k){
        return update(k, pos * 2);
    }else{
        return update(k - seg_tree[pos * 2].len, pos * 2 + 1);
    }
}

int main(int argc, char const *argv[]) {
	std::ios::sync_with_stdio(false);
    int size = 0;
    cin >> size;
    build(0, size, 1);
    for(int i = 1; i < size; i += 1){
        cin >> line[i];
    }
    for(int i = size - 1; i >= 0; i -= 1){
        res[i] = update(line[i] + 1, 1);
    }
    for(int i = 0; i < size; i += 1){
        cout << res[i] << endl;
    }
    return 0;
}
