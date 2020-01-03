#include <iostream>
using namespace std;

int n, q;
long long a[100100];

struct node{
    long long sum;
    long long delta;
    int l;
    int r;
}seg_tree[100100 << 2];

void build_tree(int s, int e, int idx){
    seg_tree[idx].l = s;
    seg_tree[idx].r = e;
    seg_tree[idx].delta = 0;
    if(s >= e - 1){
        seg_tree[idx].sum = a[s];
    	return ;
	}
    int mid = (s + e) / 2;
    build_tree(s, mid, idx * 2);
    build_tree(mid, e, idx * 2 + 1);
    seg_tree[idx].sum = seg_tree[idx * 2].sum + seg_tree[idx * 2 + 1].sum;
}

long long query(int s, int e, int idx){
    if(s <= seg_tree[idx].l && e >= seg_tree[idx].r){
        return seg_tree[idx].sum;
    }else{
        int mid = (seg_tree[idx].l + seg_tree[idx].r) / 2;
        seg_tree[idx * 2].delta += seg_tree[idx].delta;
        seg_tree[idx * 2 + 1].delta += seg_tree[idx].delta;
        seg_tree[idx * 2].sum += seg_tree[idx].delta * (mid - seg_tree[idx].l);
        seg_tree[idx * 2 + 1].sum += seg_tree[idx].delta * (seg_tree[idx].r - mid);
		seg_tree[idx].delta = 0;
        long long res = 0;
        if(s < mid){
            res += query(s, e, idx * 2);
        }
        if(e > mid){
            res += query(s, e, idx * 2 + 1);
        }
        return res;
    }
}

void add(int s, int e, int idx, long long val){
    if(s <= seg_tree[idx].l && e >= seg_tree[idx].r){
        seg_tree[idx].sum += val * (seg_tree[idx].r - seg_tree[idx].l);
        seg_tree[idx].delta += val;
    }else{
        int mid = (seg_tree[idx].l + seg_tree[idx].r) / 2;
        seg_tree[idx * 2].delta += seg_tree[idx].delta;
        seg_tree[idx * 2 + 1].delta += seg_tree[idx].delta;
        seg_tree[idx * 2].sum += seg_tree[idx].delta * (mid - seg_tree[idx].l);
        seg_tree[idx * 2 + 1].sum += seg_tree[idx].delta * (seg_tree[idx].r - mid);
        seg_tree[idx].delta = 0;
        if(s < mid){
            add(s, e, idx * 2, val);
        }
        if(e > mid){
            add(s, e, idx * 2 + 1, val);
        }
        seg_tree[idx].sum = seg_tree[idx * 2].sum + seg_tree[idx * 2 + 1].sum;
    }
}

int main(int argc, char const *argv[]) {
	std::ios::sync_with_stdio(false);
    cin >> n >> q;
    for(int i = 1; i <= n; i += 1){
        cin >> a[i];
    }
    build_tree(1, n + 1, 1);
    for(int i = 1; i <= q; i += 1){
        char com;
        cin >> com;
        if(com == 'Q'){
            int s, e;
            cin >> s >> e;
            cout << query(s, e + 1, 1) << endl;
        }else{
            int s, e, val;
            cin >> s >> e >> val;
            add(s, e + 1, 1, val);
        }
    }
    return 0;
}
