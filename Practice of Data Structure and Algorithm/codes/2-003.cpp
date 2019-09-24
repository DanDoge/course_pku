// declaration: I somehow get the spirit of this algorithm, but not really understand it

#include <iostream>
#include <algorithm>
using namespace std;

#define max_size 300010
int a[max_size], b[max_size], c[max_size];
int new_size;

inline int lowbit(int n){
    return n & (-n);
}

void update(int pos, int value){
    for(; pos <= new_size; pos += lowbit(pos)){
        c[pos] = max(c[pos], value);
    }
}

int query(int pos){
    int res = 0;
    for(; pos; pos -= lowbit(pos)){
        res = max(res, c[pos]);
    }
    return res;
}

int main(int argc, char const *argv[]) {
    ios::sync_with_stdio(false);
    int size = 0;
    cin >> size;
    for(int i = 1; i <= size; i += 1){
        cin >> a[i];
        b[i] = a[i];
    }
    sort(b + 1, b + size + 1);
    new_size = unique(b + 1, b + size + 1) - b - 1;

    for(int i = 1; i <= size; i += 1){
        // discretization: 10,20,30,30 ==> 1,2,3,3
        a[i] = lower_bound(b + 1, b + new_size + 1, a[i]) - b;
    }


    int res = 0;
    for(int i = 1; i <= size; i += 1){
        // c[i] defind as max. length of LIS ending with a[i]
        int this_res = query(a[i] - 1) + 1;
        res = max(res, this_res);
        update(a[i], this_res);
    }
    cout << res << endl;

    return 0;
}
