#include <iostream>
using namespace std;

int table[1030][1030];
int size;

int lowbit(int x){
	return x & -x;
}

void update(int x, int y, int val){
    for(int xx = x; xx <= size; xx += lowbit(xx)){
        for(int yy = y; yy <= size; yy += lowbit(yy)){
            table[xx][yy] += val;
        }
    }
}

int query(int x, int y){
    int res = 0;
    for(int xx = x; xx > 0; xx -= lowbit(xx)){
        for(int yy = y; yy > 0; yy -= lowbit(yy)){
            res += table[xx][yy];
        }
    }
    return res;
}

int main(int argc, char const *argv[]) {
    int type = 3;
    while(cin >> type && type != 3){
        if(type == 0){
            cin >> size;
        }else if(type == 2){
            int l, b, r, t;
            cin >> l >> b >> r >> t;
            cout << query(r + 1, t + 1) - query(r + 1, b) - query(l, t + 1) + query(l, b) << endl;
        }else if(type == 1){
            int x, y, a;
            cin >> x >> y >> a;
            update(x + 1, y + 1, a);
        }
    }
    return 0;
}
