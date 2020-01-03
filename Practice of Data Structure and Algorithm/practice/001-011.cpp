#include <iostream>
using namespace std;

int prec[150010];
int n, k;

void init(){
    for(int i = 1; i < 150010; i += 1){
        prec[i] = i;
    }
}

int get_prec(int idx){
    return prec[idx] == idx ? prec[idx] : prec[idx] = get_prec(prec[idx]);
}

int main(int argc, char const *argv[]) {
	std::ios::sync_with_stdio(false);
    cin >> n >> k;
    init();
    int res = 0;
    for(int i = 0; i < k; i += 1){
        int type, from, to;
        cin >> type >> from >> to;
        if(from > n || to > n || from <= 0 || to <= 0){
            res += 1;
            continue;
        }
        if(type == 1){
            if(get_prec(from) == get_prec(to + 50000)
            || get_prec(from) == get_prec(to + 2 * 50000)){
                res += 1;
            }else{
                prec[get_prec(from)] = get_prec(to);
                prec[get_prec(from + 50000)] = get_prec(to + 50000);
                prec[get_prec(from + 2 * 50000)] = get_prec(to + 2 * 50000);
            }
        }else{
            if(get_prec(from) == get_prec(to)
            || get_prec(from) == get_prec(to + 50000)){
                res += 1;
            }else{
                prec[get_prec(from)] = get_prec(to + 2 * 50000);
                prec[get_prec(from + 50000)] = get_prec(to);
                prec[get_prec(from + 2 * 50000)] = get_prec(to + 50000);
            }
        }
    }
    cout << res << endl;
    return 0;
}
