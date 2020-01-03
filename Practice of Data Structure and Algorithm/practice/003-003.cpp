#include <iostream>
#include <algorithm>
#include <memory.h>
using namespace std;

struct node{
    int l, r, poster;
}seg_tree[20100 << 2];

int num_cases;
int size;
int poster_s[20100];
int poster_e[20100];
int tmp[20100 * 3];
int vis[20100];
int tmp_idx = 0;
int ans = 0;

void build(int s, int e, int pos){
    seg_tree[pos].l = s;
    seg_tree[pos].r = e;
    seg_tree[pos].poster = 0;
    if(s >= e){
        return ;
    }
    int mid = (s + e) >> 1;
    build(s, mid, pos * 2);
    build(mid + 1, e, pos * 2 + 1);

    return ;
}

void push_down(int pos){
    if(seg_tree[pos].poster){
        seg_tree[pos * 2].poster = seg_tree[pos].poster;
        seg_tree[pos * 2 + 1].poster = seg_tree[pos].poster;
        seg_tree[pos].poster = 0;
    }
    return ;
}

void update(int s, int e, int pos, int poster){
    int l = seg_tree[pos].l;
    int r = seg_tree[pos].r;
    //cout << s << " " << e << " " << l << " " << r << endl;
    if(s <= l && e >= r){
        seg_tree[pos].poster = poster;
        return ;
    }
    if(l >= r){
        return ;
    }
    push_down(pos);
    int mid = (l + r) >> 1;
    if(s <= mid){
        update(s, e, pos * 2, poster);
    }
    if(e > mid){
        update(s, e, pos * 2 + 1, poster);
    }
}

void query(int pos){
    if(seg_tree[pos].poster){
        vis[seg_tree[pos].poster] = 1;
    }else if(seg_tree[pos].l < seg_tree[pos].r){
        query(pos * 2);
        query(pos * 2 + 1);
    }
    return ;
}

int main(int argc, char const *argv[]) {
    cin >> num_cases;
    while(num_cases--){
        cin >> size;
        tmp_idx = 0;
        memset(seg_tree, 0, sizeof(seg_tree));
        for(int i = 1; i <= size; i += 1){
            cin >> poster_s[i] >> poster_e[i];
            tmp[tmp_idx++] = poster_s[i];
            tmp[tmp_idx++] = poster_e[i];
            tmp[tmp_idx++] = poster_e[i] + 1;
        }
        sort(tmp, tmp + tmp_idx);
        int length = unique(tmp, tmp + tmp_idx) - tmp;
        build(0, length, 1);
        for(int i = 1; i <= size; i += 1){
            int s = lower_bound(tmp, tmp + length, poster_s[i]) - tmp;
            int e = lower_bound(tmp, tmp + length, poster_e[i]) - tmp;
            //cout << s << " " << e << endl;
            update(s, e, 1, i);
        }
        ans = 0;
        memset(vis, 0, sizeof(vis));
        query(1);
        for(int i = 1; i <= size; i += 1){
            ans += vis[i];
        }
        cout << ans << endl;
    }
    return 0;
}
