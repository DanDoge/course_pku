#include <iostream>
using namespace std;

long long di[100010];
long long idi[100010];
int size, q;

int lowbit(int x){
    return x & -x;
}

int add(long long idx, long long val){
    long long valdi = val;
    long long validi = val * idx;
    for(int x = idx; x <= size; x += lowbit(x)){
        di[x] += valdi;
        idi[x] += validi;
    }
    return 0;
}

long long query(int idx){
    long long resdi = 0;
    long long residi = 0;
    for(int x = idx; x > 0; x -= lowbit(x)){
        resdi += di[x];
        residi += idi[x];
    }
    return (idx + 1) * resdi - residi;
}

int main(int argc, char const *argv[]) {
    cin >> size >> q;
    long long tmpold = 0;
    for(int i = 0; i < size; i += 1){
        long long tmp;
        cin >> tmp;
        add(i + 1, tmp - tmpold);
        tmpold = tmp;
    }
    for(int i = 0; i < q; i += 1){
        char type;
        cin >> type;
        if(type == 'Q'){
            int s, e;
            cin >> s >> e;
            cout << query(e) - query(s - 1) << endl;
        }else{
            int s, e, val;
            cin >> s >> e >> val;
            add(s, val);
            add(e + 1, -val);
        }
    }
    return 0;
}
