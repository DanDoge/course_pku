#include <iostream>
using namespace std;

int prec[150010];
int size = 50000;

int init(){
    for(int i = 1; i < 150010; i += 1){
        prec[i] = i;
    }
    return 0;
}

int get_prec(int idx){
//	cout << idx << " " << prec[idx] << endl;
    return (prec[idx] == idx) ? (idx) : (prec[idx] = get_prec(prec[idx]));
}

int main(int argc, char const *argv[]) {
    int n, k, num_false = 0;
    std::ios::sync_with_stdio(false);
    cin >> n >> k;
    init();
    for(int i = 0; i < k; i += 1){
        int relation, from, to;
        cin >> relation >> from >> to;
        if(from <= 0 || from > n || to <= 0 || to > n){
        	num_false += 1;
        	continue;
		}
        if(relation == 1){
            if(get_prec(from) == get_prec(to + size)
            || get_prec(from) == get_prec(to + 2 * size)){
                num_false += 1;
            }else{
                prec[get_prec(from)] = prec[get_prec(to)];
                prec[get_prec(from + size)] = prec[get_prec(to + size)];
                prec[get_prec(from + 2 * size)] = prec[get_prec(to + 2 * size)];
            }
        }else{
            if(get_prec(from) == get_prec(to)
            || get_prec(from) == get_prec(to + size)){
                num_false += 1;
            }else{
                prec[get_prec(from)] = prec[get_prec(to + 2 * size)];
                prec[get_prec(from + size)] = prec[get_prec(to)];
                prec[get_prec(from + 2 * size)] = prec[get_prec(to + size)];
            }
        }
    }
    cout << num_false << endl;
    return 0;
}
