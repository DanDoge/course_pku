#include <iostream>
#include <memory.h>
using namespace std;

int prec[50010];
int num_cola = 0;

void init(int size){
    for(int i = 1; i <= size; i += 1){
        prec[i] = i;
    }
    return ;
}

int get_prec(int pos){
    if(prec[pos] == pos){
        return pos;
    }
    return prec[pos] = get_prec(prec[pos]);
}

void merge(int to, int from){
    int prec_to = get_prec(to);
    int prec_from = get_prec(from);
    if(prec_to == prec_from){
        cout << "Yes" << endl;
    }else{
        prec[prec_from] = prec_to;
        num_cola -= 1;
        cout << "No" << endl;
    }
}

int main(int argc, char const *argv[]) {
    std::ios::sync_with_stdio(false);
    int size, num_op;
    while(cin >> size >> num_op){
        init(size);
        num_cola = size;
        while(num_op--){
            int to, from;
            cin >> to >> from;
            merge(to, from);
        }
        cout << num_cola << endl;
        int first = 1;
        for(int i = 1; i <= size; i += 1){
            if(get_prec(i) == i){
                (first == 1) ? (first = 0) : (cout << " ");
                cout << i;
            }
        }
        cout << endl;
    }
    return 0;
}
