#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int arr[100010];
int sorted[100010];
vector<int> seg_tree[(1 << 18)];
int size;
int padding;

void build_tree(int pos, int s, int e){
    if(s + 1 >= e){
        if(s <= size){
            seg_tree[pos].push_back(arr[s]);
        }
        return ;
    }

    int m = (s + e) >> 1;
    int l = pos << 1;
    int r = (pos << 1) + 1;
    build_tree(l, s, m);
    build_tree(r, m, e);
    seg_tree[pos].resize(e - s);
    merge(seg_tree[l].begin(), seg_tree[l].end(), seg_tree[r].begin(), seg_tree[r].end(), seg_tree[pos].begin());
    return ;
}

int query(int s, int e, int pos, int val, int l, int r){
    if(l >= e || r <= s){
        return 0;
    }
    if(s <= l && r <= e){
        return upper_bound(seg_tree[pos].begin(), seg_tree[pos].end(), val) - seg_tree[pos].begin();
    }
    int m = (l + r) >> 1;
    int left_cnt = query(s, e, pos << 1, val, l, m);
    int right_cnt = query(s, e, (pos << 1) + 1, val, m, r);
    return left_cnt + right_cnt;
}

int bin_search(int s, int e, int k){
    int l = 1;
    int r = size;
    if(query(s, e, 1, sorted[l], 1, padding + 1) >= k){
    	return sorted[l];
	}
    while(r - l > 1){
        int mid = (l + r) >> 1;
		if(query(s, e, 1, sorted[mid], 1, padding + 1) >= k){
            r = mid;
        }else{
            l = mid;
        }
    }
    return sorted[r];
}

int main(int argc, char const *argv[]) {
    ios::sync_with_stdio(false);
    int num_query = 0;
    cin >> size >> num_query;
    for(int i = 1; i <= size; i += 1){
        cin >> arr[i];
        sorted[i] = arr[i];
    }
    sort(sorted + 1, sorted + size + 1);
    padding = 1;
    while(padding < size){
    	padding <<= 1;
	}
    build_tree(1, 1, padding + 1);
    while(num_query--){
        int s, e, k;
        cin >> s >> e >> k;
        cout << bin_search(s, e + 1, k) << endl;
    }
    return 0;
}
