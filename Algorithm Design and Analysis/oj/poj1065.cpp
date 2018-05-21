#include <iostream>
#include <algorithm>
#include <memory.h>
using namespace std;

struct wood{
    int w;
    int l;
    wood(){
        w = l = 0;
    }
    bool operator<(const wood& obj){
        return w < obj.w || (w == obj.w && l < obj.l);
    }
}woods[5010];

int used[5010] = {};

int main(int argc, char const *argv[]) {
    std::ios::sync_with_stdio(false);
    int casecnt = 0;
    cin >> casecnt;
    while(casecnt--){
        int n = 0;
        cin >> n;
        for(int i = 0; i < n; i += 1){
            cin >> woods[i].l >> woods[i].w;
        }
        sort(woods, woods + n);
        int prep = 0;
        int maxlength = 1;
        int ans = 0;
        memset(used, 0, sizeof(used));
        for(int i = 0; i < n; i += 1){
            if(!used[i]){
                ans += 1;
                used[i] = 1;
                int tmpl = woods[i].l;
                for(int j = i + 1; j < n; j += 1){
                    if(!used[j] && tmpl <= woods[j].l){
                        //cout << i << " " << j << endl;
                        used[j] = 1;
                        tmpl = woods[j].l;
                    }
                }
            }
        }
        cout << ans << endl;
    }
    return 0;
}
