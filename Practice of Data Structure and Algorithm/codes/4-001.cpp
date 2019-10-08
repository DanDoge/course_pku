#include <iostream>
#include <memory.h>
#include <algorithm>
using namespace std;

int left_end[20010], right_end[20010];
int col[20010 << 2], vis[20010];
int tmp[20010 * 3];
int ans = 0;

int compress(int num_pters){
    int idx = 0;
    for(int i = 0; i < num_pters; i += 1){
        tmp[idx++] = left_end[i];
        tmp[idx++] = right_end[i];
        tmp[idx++] = right_end[i] + 1;
    }
    sort(tmp, tmp + idx);
    return unique(tmp, tmp + idx) - tmp;
}

void push_down(int p){
    if(col[p] == -1){
        return ;
    }else{
        col[p << 1] = col[(p << 1) + 1] = col[p];
        col[p] = -1;
        return ;
    }
}

void insert(int p, int s, int e, int l, int r, int val){
    if(s >= l && e <= r){
        col[p] = val;
        return ;
    }
    push_down(p);
    int mid = (s + e) >> 1;
    if(l < mid){
        insert(p << 1, s, mid, l, r, val);
    }
    if(r > mid){
        insert((p << 1) + 1, mid, e, l, r, val);
    }
}

void query(int p, int s, int e){
    if(col[p] != -1){
        if(!vis[col[p]]){
            vis[col[p]] = 1;
            ans += 1;
        }
        return ;
    }
    int mid = (s + e) >> 1;
    query(p << 1, s, mid);
    query((p << 1) + 1, mid, e);
}

int main(int argc, char const *argv[]) {
    ios::sync_with_stdio(false);
    int num_cases;
    cin >> num_cases;
    while(num_cases--){
        memset(col, 0, sizeof(col));
        memset(vis, 0, sizeof(vis));
        int num_pters;
        cin >> num_pters;
        for(int i = 0; i < num_pters; i += 1){
            cin >> left_end[i] >> right_end[i];
            right_end[i] += 1;
        }
        int unique_ele = compress(num_pters);
        for(int i = 0; i < num_pters; i += 1){
            int l = lower_bound(tmp, tmp + unique_ele, left_end[i]) - tmp;
            int r = lower_bound(tmp, tmp + unique_ele, right_end[i]) - tmp;
            insert(1, 1, unique_ele + 1, l + 1, r + 1, i + 1);
        }
        vis[0] = 1;
        query(1, 1, unique_ele);
        cout << ans << endl;
        ans = 0;
    }
    return 0;
}
