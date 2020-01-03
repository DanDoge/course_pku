#include <iostream>
using namespace std;

int n, q;
long long a[100010];
struct node{
    int l, r;
    long long sum;
    long long delta;
}seg_tree[100010 << 2];

void push_up(int pos){
    seg_tree[pos].sum = seg_tree[pos * 2].sum + seg_tree[pos * 2 + 1].sum;
    return ;
}

void push_down(int pos){
    seg_tree[pos * 2].delta += seg_tree[pos].delta;
	seg_tree[pos * 2 + 1].delta += seg_tree[pos].delta;
    seg_tree[pos * 2].sum += seg_tree[pos].delta * (seg_tree[pos * 2].r - seg_tree[pos * 2].l);
    seg_tree[pos * 2 + 1].sum += seg_tree[pos].delta * (seg_tree[pos * 2 + 1].r - seg_tree[pos * 2 + 1].l);
    seg_tree[pos].delta = 0;
    return ;
}

void build(int s, int e, int pos){
    seg_tree[pos].l = s;
    seg_tree[pos].r = e;
    seg_tree[pos].sum = 0;
    seg_tree[pos].delta = 0;
    if(s >= e - 1){
        seg_tree[pos].sum = a[s];
        return ;
    }
    int mid = (s + e) >> 1;
    build(s, mid, pos * 2);
    build(mid, e, pos * 2 + 1);
    push_up(pos);
    return ;
}

void update(int s, int e, int pos, int val){
    if(s <= seg_tree[pos].l && seg_tree[pos].r <= e){
        seg_tree[pos].delta += val;
        seg_tree[pos].sum += val * (seg_tree[pos].r - seg_tree[pos].l);
        //cout << s << " " << e << " " << seg_tree[pos].l << " " << seg_tree[pos].r << " " << seg_tree[pos].delta << " " << seg_tree[pos].sum << endl;
        return ;
    }
    push_down(pos);
    int mid = (seg_tree[pos].r + seg_tree[pos].l) >> 1;
    if(s < mid){
        update(s, e, pos * 2, val);
    }
    if(e > mid){
        update(s, e, pos * 2 + 1, val);
    }
    push_up(pos);
    //cout << s << " " << e << " " << seg_tree[pos].l << " " << seg_tree[pos].r << " " << seg_tree[pos].delta << " " << seg_tree[pos].sum << endl;
    return ;
}

long long query(int s, int e, int pos){
    if(s <= seg_tree[pos].l && seg_tree[pos].r <= e){
    	//cout << s << " " << e << " " << seg_tree[pos].l << " " << seg_tree[pos].r << " " << seg_tree[pos].delta << " " << seg_tree[pos].sum << endl;
        return seg_tree[pos].sum;
    }
    push_down(pos);
    long long res = 0;
    int mid = (seg_tree[pos].r + seg_tree[pos].l) >> 1;
    if(s < mid){
        res += query(s, e, pos * 2);
    }
    if(e> mid){
        res += query(s, e, pos * 2 + 1);
    }
    push_up(pos);
    //cout << s << " " << e << " " << seg_tree[pos].l << " " << seg_tree[pos].r << " " << seg_tree[pos].delta << " " << seg_tree[pos].sum << endl;
    return res;
}

int main(int argc, char const *argv[]) {
    cin >> n >> q;
    for(int i = 0; i < n; i += 1){
        cin >> a[i];
    }
    build(0, n, 1);
    while(q--){
        char com;
        cin >> com;
        if(com == 'Q'){
            int s, e;
            cin >> s >> e;
            cout << query(s - 1, e, 1) << endl;
        }else{
            int s, e, val;
            cin >> s >> e >> val;
            update(s - 1, e, 1, val);
        }
    }
    return 0;
}
