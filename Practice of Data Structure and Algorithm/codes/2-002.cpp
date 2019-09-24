#include <iostream>
#include <vector>
using namespace std;

vector<vector<long long> > c;
int size;

int lowbit(int n){
    return n & (-n);
}

void update(int pos_x, int pos_y, long long a){
    for(int x = pos_x; x <= size; x += lowbit(x)){
        for(int y = pos_y; y <= size; y += lowbit(y)){
            c[x][y] += a;
        }
    }
    return ;
}

long long query(int pos_x, int pos_y){
    long long sum = 0;
    for(int x = pos_x; x > 0; x -= lowbit(x)){
        for(int y = pos_y; y > 0; y -= lowbit(y)){
            sum += c[x][y];
        }
    }
    return sum;
}

int main(int argc, char const *argv[]) {
    std::ios::sync_with_stdio(false);
    int type_instr = 0;
    cin >> type_instr >> size;
    c = vector<vector<long long> >(size + 1, vector<long long>(size + 1, 0));

    while(cin >> type_instr){
        switch (type_instr) {
            case 1 : {
                int x, y, a;
                cin >> x >> y >> a;
                update(x + 1, y + 1, a);
                break;
            }
            case 2 : {
                int l, b, r, t;
                cin >> l >> b >> r >> t;
                r += 1;
                t += 1;
                cout << query(r, t) - query(r, b) - query(l, t) + query(l, b) << endl;
                break;
            }
            case 3 : {
                return 0;
            }
        }
    }
    return 0;
}
