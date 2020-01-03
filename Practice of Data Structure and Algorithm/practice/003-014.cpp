#include <iostream>
#include <algorithm>
#include <memory.h> 
using namespace std;

struct node{
    int l, r, p;
}seg[40010 << 4];

int l[10010], r[10010], tmp[30030];
int vis[10010];
int tmpidx = 0;
int n;

void build(int s, int e, int pos){
    seg[pos].l = s;
    seg[pos].r = e;
    seg[pos].p = 0;
    if(s >= e - 1){
        return ;
    }
    int mid = (s + e) / 2;
    build(s, mid, pos * 2);
    build(mid, e, pos * 2 + 1);
    return ;
}

void update(int s, int e, int pos, int p){
    if(s <= seg[pos].l && seg[pos].r <= e){
        seg[pos].p = p;
        return ;
    }
    if(seg[pos].p){
		seg[pos * 2].p = seg[pos * 2 + 1].p = seg[pos].p;
    	seg[pos].p = 0;	
	}
    int mid = (seg[pos].l + seg[pos].r) / 2;
    if(s < mid){
        update(s, e, pos * 2, p);
    }
    if(mid < e){
        update(s, e, pos * 2 + 1, p);
    }
    return ;
}

void query(int s, int e, int pos){
    if(seg[pos].p > 0){
        vis[seg[pos].p] = 1;
        return ;
    }
    if(seg[pos].l >= seg[pos].r - 1){
    	return ;
	}
    int mid = (seg[pos].l + seg[pos].r) / 2;
    if(s < mid){
        query(s, e, pos * 2);
    }
    if(mid < e){
        query(s, e, pos * 2 + 1);
    }
    return ;
}

int main(int argc, char const *argv[]) {
    int casecnt = 0;
    cin >> casecnt;
    while(casecnt--){
        cin >> n;
        for(int i = 0; i < n; i += 1){
            cin >> l[i] >> r[i];
            tmp[tmpidx++] = l[i];
            tmp[tmpidx++] = r[i];
            tmp[tmpidx++] = r[i] + 1;
        }
        sort(tmp, tmp + tmpidx);
        int length = unique(tmp, tmp + tmpidx) - tmp;
        build(1, length + 1, 1);
        for(int i = 0; i < n; i += 1){
            int le = lower_bound(tmp, tmp + length, l[i]) - tmp;
            int re = lower_bound(tmp, tmp + length, r[i]) - tmp;
            update(le + 1, re + 2, 1, i + 1);
        }
        query(1, length, 1);
        int res = 0;
        for(int i = 1; i <= n; i += 1){
            res += vis[i];
        }
        cout << res<< endl;
        memset(seg, 0, sizeof(seg));
        memset(l, 0, sizeof(l));
        memset(r, 0, sizeof(r));
        memset(tmp, 0, sizeof(tmp));
        memset(vis, 0, sizeof(vis));
        tmpidx = 0;
    }
    return 0;
}
