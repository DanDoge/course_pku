#include <iostream>
using namespace std;

int cola[50010];
int num_cola;

int init(int n, int m){
    for(int i = 1; i <= n; i += 1){
        cola[i] = i;
    }
    num_cola = n;
    return 0;
}

int get_prec(int idx){
    if(cola[idx] == idx){
        return idx;
    }else{
        return cola[idx] = get_prec(cola[idx]);
    }
    return -1;
}

int merge(int from, int to){
    if(get_prec(from) == get_prec(to)){
        cout << "Yes" << endl;
        return 0;
    }else{
        num_cola -= 1;
        cout << "No" << endl;
        cola[from] = cola[to];
        return 0;
    }
}

int main(int argc, char const *argv[]) {
    int n, m;
    while(cin >> n >> m){
        init(n, m);
        for(int i = 0; i < m; i += 1){
            int from, to;
            cin >> to >> from;
            merge(from, to);
        }
        cout << num_cola << endl;
        int first = 1;
        for(int i = 1; i <= n; i += 1){
            if(cola[i] == i){
                if(first){
                    first = 0;
                }else{
                    cout << " ";
                }
                cout << i;
            }
        }
        cout << endl;
    }
    return 0;
}
