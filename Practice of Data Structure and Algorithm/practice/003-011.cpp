#include <iostream>
#include <algorithm>
#include <memory.h>
using namespace std;

struct node{
    int l, r, poster;
}segtree[40010 << 2];

int vis[10010];
int l[10010];
int r[10010];
int tmp[40010];
int tmpidx;

void build(int s, int e, int pos){
    segtree[pos].l = s;
    segtree[pos].r = e;
    segtree[pos].poster = 0;
    if(s >= e - 1){
        return ;
    }
    int mid = (s + e) / 2;
    build(s, mid, pos * 2);
    build(mid, e, pos * 2 + 1);
    return ;
}

void push_down(int pos){
	if(!segtree[pos].poster){
		return ;
	}
    segtree[pos * 2].poster = segtree[pos * 2 + 1].poster = segtree[pos].poster;
    segtree[pos].poster = 0;
    return ;
}

void update(int s, int e, int pos, int poster){
	//cout << segtree[pos].l << " " << segtree[pos].r << ' ' << s << " " << e << endl;
    if(s <= segtree[pos].l && segtree[pos].r <= e){
        segtree[pos].poster = poster;
        return ;
    }
    push_down(pos);
    int mid = (segtree[pos].l + segtree[pos].r) / 2;
    if(s < mid){
        update(s, e, pos * 2, poster);
    }
    if(e > mid){
        update(s, e, pos * 2 + 1, poster);
    }
    return ;
}

void query(int pos){
    //cout << pos << endl;
    if(segtree[pos].poster){
        vis[segtree[pos].poster] = 1;
        return ;
    }
    if(segtree[pos].l >= segtree[pos].r - 1){
        return ;
    }
    query(pos * 2);
    query(pos * 2 + 1);
}

int main(int argc, char const *argv[]) {
    int numcases = 0;
    cin >> numcases;
    while(numcases--){
        int numposters;
        cin >> numposters;
        for(int i = 0; i < numposters; i += 1){
            cin >> l[i] >> r[i];
            tmp[tmpidx++] = l[i];
            tmp[tmpidx++] = r[i];
            tmp[tmpidx++] = r[i] + 1;
        }
        sort(tmp, tmp + tmpidx);
        int length = unique(tmp, tmp + tmpidx) - tmp;
        build(1, length + 1, 1);
        for(int i = 0; i < numposters; i += 1){
            int pl = lower_bound(tmp, tmp + length, l[i]) - tmp;
            int pr = lower_bound(tmp, tmp + length, r[i]) - tmp;
            update(pl + 1, pr + 2, 1, i + 1);
        }
        query(1);
        int res = 0;
        for(int i = 1; i <= numposters; i += 1){
            res += vis[i];
        }
        cout << res << endl;
        memset(vis, 0, sizeof(vis));
        memset(l, 0, sizeof(l));
        memset(r, 0, sizeof(r));
        memset(tmp, 0, sizeof(tmp));
        memset(segtree, 0, sizeof(segtree));
        tmpidx = 0;
    }
    return 0;
}
