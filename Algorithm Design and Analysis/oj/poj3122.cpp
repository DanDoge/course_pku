#include <iostream>
#include <memory.h>
#include <iomanip>
using namespace std;

const double pi = 3.1415926535897932;

double r[10010] = {};

int main(){
    std::ios::sync_with_stdio(false);
    int casesize = 0;
    cin >> casesize;
    while(casesize--){
        int n, f;
        double s = 0.0f, e = 0.0f;
        cin >> n >> f;
        memset(r, 0, sizeof(r));
        for(int i = 0; i < n; i += 1){
            int tmp = 0;
            cin >> tmp;
            e += (r[i] = tmp * tmp * pi);
        }
        e /= f;
        while(e - s > 0.0001){
            double mid = (e + s) / 2;
            int cnt = 0;
            for(int i = 0; i < n; i += 1){
                cnt += r[i] / mid;
            }
            if(cnt > f){
                s = mid;
            }else{
                e = mid;
            }
        }
        cout << fixed << setprecision(4) << s << endl;
    }
    return 0;
}
