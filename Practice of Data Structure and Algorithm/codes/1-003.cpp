#include <iostream>
using namespace std;

int prec[4001];

#define prec_from get_prec(from)
#define prec_to get_prec(to)
#define prec_from_eat get_prec(from + size)
#define prec_to_eat get_prec(to + size)

void init(int size){
    for(int i = 0; i <= 2 * size + 2; i += 1){
        prec[i] = i;
    }
}

int get_prec(int pos){
    return (prec[pos] == pos) ? (pos) : (prec[pos] = get_prec(prec[pos]));
}

int main(int argc, char const *argv[]) {
    std::ios::sync_with_stdio(false);
    int num_cases;
    cin >> num_cases;
    int first_case = 1;
    for(int case_num = 1; case_num <= num_cases; case_num += 1){
        if(first_case == 1){
        	first_case = 0;
		}else{
			cout << endl;
		}
        cout << "Scenario #" << case_num << ":" << endl;
        int size, num_statement, num_fallacy = 0;
        cin >> size >> num_statement;
        init(size);
        while(num_statement--){
            int from, to;
            cin >> from >> to;

            if(num_fallacy > 0){
                continue;
            }

            if(prec_from == prec_to
               || prec_from_eat == prec_to_eat){
                num_fallacy += 1;
            }else{
                prec[prec_from] = prec_to_eat;
                prec[prec_from_eat] = prec_to;
            }
        }
        if(num_fallacy > 0){
            cout << "Suspicious bugs found!" << endl;
        }else{
            cout << "No suspicious bugs found!" << endl;
        }
    }
    return 0;
}
