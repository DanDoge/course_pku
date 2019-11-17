#include <iostream>
#include <set>
#include <vector>
#include <memory.h>
#include <algorithm>
using namespace std;

int low[1010], dfn[1010], cut[1010], dfn[1010];
int depth;
int casenum;

vector<set<int> > net;
vector<int> res;

void tarjan(int idx, int fa){
    dfn[idx] = low[idx] = ++depth;
    int num_child = 0;
    for(set<int>::iterator it = net[idx].begin(); it != net[idx].end(); it++){
        if(!dfn[*it]){
            num_child += 1;
            tarjan(*it, idx);
            low[idx] = min(low[idx], low[*it]);
            if(low[*it] >= dfn[idx]){
                cut[idx] += 1;
            }
        }else if(*it != fa){
            low[idx] = min(low[idx], dfn[*it]);
        }
    }
    if(fa < 0){
        if(num_child >= 2){
            res.push_back(idx);
        }
    }else{
        if(cut[idx] >= 1){
            res.push_back(idx);
            cut[idx] += 1;
        }
    }
    return ;
}

int main(int argc, char const *argv[]) {
    int u, v;
    while(cin >> u && u){
        depth = 0;
        memset(dfn, 0, sizeof(dfn));
        memset(cut, 0, sizeof(cut));
        res.clear();
        net.resize(1010);
        for(int i = 0; i < 1010; i += 1){
            net[i].clear();
        }
        do{
            cin >> v;
            net[u].insert(v);
            net[v].insert(u);
            cin >> u;
        }while(u);
        for(int i = 1; i < 1010; i += 1){
            if(dfn[i] == 0 && net[i].size()){
                tarjan(i, -1);
            }
        }
        sort(&res[0], &res[0] + res.size());
        if(res.size()){
            cout << "Network #" << ++casenum << endl;
            for(int i = 0; i < res.size(); i += 1){
                cout << "  SPF node " << res[i] << " leaves " << cut[res[i]] << " subnets" << endl;
            }
        }else{
            cout << "Network #" << ++casenum << endl << "  No SPF nodes" << endl;
        }
        cout << endl;
    }
     return 0;
}
