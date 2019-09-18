#include <iostream>
using namespace std;

int prec[150001];

#define prec_from get_prec(from)
#define prec_to get_prec(to)
#define prec_from_eat get_prec(from + size)
#define prec_to_eat get_prec(to + size)
#define prec_eat_from get_prec(from + 2 * size)
#define prec_eat_to get_prec(to + 2 * size)

void init(int size){
    for(int i = 0; i <= 3 * size + 3; i += 1){
        prec[i] = i;
    }
}

int get_prec(int pos){
    return (prec[pos] == pos) ? (pos) : (prec[pos] = get_prec(prec[pos]));
}

int main(int argc, char const *argv[]) {
    std::ios::sync_with_stdio(false);
    int size, num_statement, num_fallacy = 0;
    cin >> size >> num_statement;
    init(size);
    while(num_statement--){
        int relation, from, to;
        cin >> relation >> from >> to;

        if(from <= 0 || from > size || to <= 0 || to > size){
            num_fallacy += 1;
            continue;
        }

        if(relation == 1){
            if(prec_from == prec_eat_to
               || prec_from == prec_to_eat){
                num_fallacy += 1;
            }else{
                prec[prec_from] = prec_to;
                prec[prec_from_eat] = prec_to_eat;
                prec[prec_eat_from] = prec_eat_to;
            }
        }else{
            if(prec_from == prec_to
               || prec_from == prec_to_eat){
                num_fallacy += 1;
            }else{
                prec[prec_from] = prec_eat_to;
                prec[prec_from_eat] = prec_to;
                prec[prec_eat_from] = prec_to_eat;
            }
        }
    }
    cout << num_fallacy << endl;
    return 0;
}
