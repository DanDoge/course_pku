#include <iostream>
using namespace std;

int prec[4096];
int size = 2001;
int n, m;

int init(){
    for(int i = 1; i <= 4096; i += 1){
        prec[i] = i;
    }
    return 0;
}

int get_prec(int idx){
    return prec[idx] == idx ? idx : prec[idx] = get_prec(prec[idx]);
}

int main(int argc, char const *argv[]) {
    std::ios::sync_with_stdio(false);
    int num_cases;
    cin >> num_cases;
    for(int cas = 1; cas <= num_cases; cas += 1){
        if(cas > 1){
            cout << endl;
        }
        cout << "Scenario #" << cas << ":" << endl;
        cin >> n >> m;
        init();
        int sus = 0;
        for(int i = 0; i < m; i += 1){
            int from, to;
            cin >> from >> to;
            if(get_prec(from) == get_prec(to)){
                sus = 1;
            }else{
                prec[get_prec(from)] = get_prec(to + size);
                prec[get_prec(from + size)] = get_prec(to);
            }
        }
        if(sus){
            cout << "Suspicious bugs found!" << endl;
        }else{
            cout << "No suspicious bugs found!" << endl;
        }
    }
    return 0;
}
