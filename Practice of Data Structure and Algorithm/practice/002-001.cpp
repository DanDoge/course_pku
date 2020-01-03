#include <iostream>
#include <vector>
using namespace std;

std::vector<vector<int> > tree;
int s[100010], e[100010];
char apple[200010];
int c[200010];
int dfs_idx = 0;
int num_forks = 0;

void dfs(int idx){
    s[idx] = ++dfs_idx;
    for(int i = 0; i < tree[idx].size(); i += 1){
        dfs(tree[idx][i]);
    }
    e[idx] = ++dfs_idx;
    return ;
}

int lowbit(int x){
    return x & -x;
}

void add(int pos){
    int delta = apple[pos];
    for(; pos <= 2 * num_forks; pos += lowbit(pos)){
        c[pos] += delta;
    }
    return ;
}

int query(int pos){
    int sum = 0;
    for(; pos > 0; pos -= lowbit(pos)){
        sum += c[pos];
    }
    return sum;
}

int main(int argc, char const *argv[]) {
    std::ios::sync_with_stdio(false);
    cin >> num_forks;
    tree.resize(num_forks + 1);
    for(int i = 1; i <= num_forks; i += 1){
        tree[i].resize(0);
    }
    for(int i = 1; i < num_forks; i += 1){
        int s, e;
        cin >> s >> e;
        tree[s].push_back(e);
    }
    dfs(1);
    for(int i = 1; i <= num_forks; i += 1){
        apple[s[i]] = 1;
        apple[e[i]] = 1;
        add(s[i]);
        add(e[i]);
    }
    int num_query = 0;
    cin >> num_query;
    while(num_query--){
        char mes;
        cin >> mes;
        if(mes == 'Q'){
            int pos = 0;
            cin >> pos;
            int res = query(e[pos]) - query(s[pos] - 1);
            cout << res / 2 << endl;
        }else{
            int pos = 0;
            cin >> pos;
            apple[s[pos]] = -apple[s[pos]];
            apple[e[pos]] = -apple[e[pos]];
            add(s[pos]);
            add(e[pos]);
        }
    }
    return 0;
}
