#include <iostream>
#include <algorithm>
using namespace std;

int main(){
    int cases = 0;
    cin >> cases;
    while(cases--){
        int n = 0;
        cin >> n;
        int a[1024] = {};
        for(int i = 0; i < n; i += 1){
            cin >> a[i];
        }
        sort(a, a + n);
        int length = n;
        int t = 0;
        while(length >= 4){
            t += min(a[1] * 2, a[0] + a[length - 2]) + a[0] + a[length - 1];
            length -= 2;
        }
        if(length == 1){
            t += a[0];
        }else if(length == 2){
            t += a[1];
        }else if(length == 3){
            t += a[0] + a[1] + a[2];
        }
        cout << t << endl;
    }
}
