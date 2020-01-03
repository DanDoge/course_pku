#include <iostream>
using namespace std;

int n, q;

long long d[100010];
long long idi[100010];

int lowbit(int x){
    return x & -x;
}

void add(long long* a, int pos, long long val){
    for(; pos <= n; pos += lowbit(pos)){
        a[pos] += val;
    }
    return ;
}

long long query(long long* a, int pos){
    long long sum = 0;
    for(; pos > 0; pos -= lowbit(pos)){
        sum += a[pos];
    }
    return sum;
}

int main(int argc, char const *argv[]) {
	std::ios::sync_with_stdio(false);
    cin >> n >> q;
    long long past = 0;
    for(int i = 1; i <= n; i += 1){
        long long now = 0;
        cin >> now;
        add(d, i, now - past);
        add(idi, i, (now - past) * i);
        past = now;
    }
    for(int i = 1; i <= q; i += 1){
        char com;
        cin >> com;
        if(com == 'Q'){
            long long s, e;
            cin >> s >> e;
            long long e_sum = (e + 1) * query(d, e) - query(idi, e);
            long long s_sum = s * query(d, s - 1) - query(idi, s - 1);
            cout << e_sum - s_sum << endl;
        }else{
            long long s, e, val;
            cin >> s >> e >> val;
            add(d, s, val);
            add(d, e + 1, -val);
            add(idi, s, val * s);
            add(idi, e + 1, -(e + 1) * val);
        }
    }
    return 0;
}
