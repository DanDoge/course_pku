#include <iostream>
using namespace std;

int c[1025][1025];
int size;

int lowbit(int x){
	return x & -x;
}

int add(int x, int y, int val){
    for(int xx = x; xx <= size; xx += lowbit(xx)){
        for(int yy = y; yy <= size; yy += lowbit(yy)){
            c[xx][yy] += val;
        }
    }
    return 0;
}

int query(int x, int y){
    int sum = 0;
    for(int xx = x; xx > 0; xx -= lowbit(xx)){
        for(int yy = y; yy > 0; yy -= lowbit(yy)){
            sum += c[xx][yy];
        }
    }
    return sum;
}

int main(int argc, char const *argv[]) {
    std::ios::sync_with_stdio(false);
    int type;
    while(cin >> type && type != 3){
        if(type == 0){
            cin >> size;
        }else if(type == 1){
            int x, y, val;
            cin >> x >> y >> val;
            add(x + 1, y + 1, val);
        }else if(type == 2){
            int l, b, r, t;
            cin >> l >> b >> r >> t;
            cout << query(r + 1, t + 1) - query(r + 1, b) - query(l, t + 1) + query(l, b) << endl;
        }
    }
    return 0;
}
