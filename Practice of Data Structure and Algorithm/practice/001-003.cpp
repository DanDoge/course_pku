#include <iostream>
using namespace std;

int prec[4048];
int size = 2000;

int init(){
    for(int i = 1; i < 4048; i += 1){
        prec[i] = i;
    }
    return 0;
}

int get_prec(int idx){
    return (prec[idx] == idx) ? idx : (prec[idx] = get_prec(prec[idx]));
}

int main(int argc, char const *argv[]) {
    std::ios::sync_with_stdio(false);
    int num_cases = 0;
    cin >> num_cases;
    for(int i = 1; i <= num_cases; i += 1){
    	if(i > 1){
            cout << endl;
        }
        cout << "Scenario #" << i << ":" << endl;
        int num_bug, num_inter;
        cin >> num_bug >> num_inter;
        init();
        int sus = 0;
        for(int j = 1; j <= num_inter; j += 1){
            int from, to;
            cin >> from >> to;
            int from_prec = get_prec(from);
            int from_inter_prec = get_prec(from + size);
            int to_prec = get_prec(to);
            int to_inter_prec = get_prec(to + size);
            if(from_prec == to_prec || from_inter_prec == to_inter_prec){
                sus = 1;
            }else{
                prec[from_prec] = to_inter_prec;
                prec[from_inter_prec] = to_prec;
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
