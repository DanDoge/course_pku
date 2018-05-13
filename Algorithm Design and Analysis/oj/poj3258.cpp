#include <iostream>
#include <algorithm>
using namespace std;

int l = 0;
int n = 0;
int m = 0;
int a[50010] = {};

int f(int k){
    int cnt = 0;
    int pre = 0;
    for(int i = 1; i <= n; i += 1){
        if(a[i] - a[pre] < k){
            cnt += 1;
        }else{
            pre = i;
        }
    }
    if(a[n + 1] - a[pre] < k){
        cnt += 1;
    }
    return cnt;
}

int main() {
    std::ios::sync_with_stdio(false);
    cin >> l >> n >> m;
    for(int i = 1; i <= n; i += 1){
        cin >> a[i];
    }
    a[0] = 0, a[n + 1] = l;
    sort(a + 1, a + n + 1);
    int s = 0, e = l + 1;
    int ans = 0;
    while(s < e){
        int mid = (s + e) / 2;
        if(f(mid) <= m){
            s = mid + 1;
            ans = mid;
        }else{
            e = mid;
        }
    }
    cout << ans << endl;
    return 0;
}
