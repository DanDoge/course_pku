#include <iostream>
using namespace std;

int main() {
    long long n, s;
    cin >> n >> s;
    long long ans = 0;
    long long min_cost = 19260817;
    while(n--){
        long long ci, yi;
        cin >> ci >> yi;
        min_cost = min(min_cost + s, ci);
        ans += min_cost * yi;
    }
    cout << ans << endl;
    return 0;
}
