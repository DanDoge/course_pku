#include <iostream>
using namespace std;

struct node{
    int l, r;
    long long sum, delta;
}seg[100010 << 2];

long long a[100010];

int n, q;

void pushup(int pos){
    seg[pos].sum = seg[pos * 2].sum + seg[pos * 2 + 1].sum;
    return ;
}

void pushdown(int pos){
    if(seg[pos].delta){
        seg[pos * 2].delta += seg[pos].delta;
        seg[pos * 2 + 1].delta += seg[pos].delta;
        seg[pos * 2].sum += seg[pos].delta * (seg[pos * 2].r - seg[pos * 2].l);
		seg[pos * 2 + 1].sum += seg[pos].delta * (seg[pos * 2 + 1].r - seg[pos * 2 + 1].l);
        seg[pos].delta = 0;
    }
    return ;
}

void build(int s, int e, int pos){
    seg[pos].l = s;
    seg[pos].r = e;
    seg[pos].sum = 0;
    seg[pos].delta = 0;
    if(s >= e - 1){
        seg[pos].sum = a[s];
        return ;
    }
    int mid = (s + e) / 2;
    build(s, mid, pos * 2);
    build(mid, e, pos * 2 + 1);
    pushup(pos);
    return ;
}

void update(int s, int e, int pos, int val){
    if(s <= seg[pos].l && seg[pos].r <= e){
        seg[pos].sum += val * (seg[pos].r - seg[pos].l);
        seg[pos].delta += val;
        return ;
    }
    pushdown(pos);
    if(seg[pos].l >= seg[pos].r - 1){
        return ;
    }
    int mid = (seg[pos].l + seg[pos].r) / 2;
    if(s < mid){
        update(s, e, pos * 2, val);
    }
    if(e > mid){
        update(s, e, pos * 2 + 1, val);
    }
    pushup(pos);
    return ;
}

long long query(int s, int e, int pos){
    if(s <= seg[pos].l && seg[pos].r <= e){
        return seg[pos].sum;
    }
    pushdown(pos);
    long long res = 0;
    int mid = (seg[pos].l + seg[pos].r) / 2;
    if(s < mid){
        res += query(s, e, pos * 2);
    }
    if(e > mid){
        res += query(s, e, pos * 2 + 1);
    }
    return res;
}

int main(int argc, char const *argv[]) {
    cin >> n >> q;
    for(int i = 0; i < n; i += 1){
        cin >> a[i];
    }
    build(0, n, 1);
    for(int i = 0; i < q; i += 1){
        char type;
        cin >> type;
        if(type == 'Q'){
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
