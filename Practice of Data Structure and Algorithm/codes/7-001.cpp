#include <iostream>
#include <vector>
#include <set>
#include <memory.h>
using namespace std;

vector<set<int> > pop;

int depth = 0, top = 0, comp = 0;
int dfn[10010], low[10010], ins[10010], stack[10010], belong[10010];
int in_deg[10010], out_deg[10010];

void tarjan(int i){
    stack[top++] = i;
    ins[i] = 1;
    dfn[i] = low[i] = ++depth;
    for(set<int>::iterator it=pop[i].begin(); it != pop[i].end(); it++){
        if(!dfn[*it]){
            tarjan(*it);
            low[i] = min(low[i], low[*it]);
        }else if(ins[*it]){
            low[i] = min(low[i], dfn[*it]);
        }
    }
    if(dfn[i] == low[i]){
        while(true){
            int tmp = stack[--top];
            ins[tmp] = 0;
            belong[tmp] = comp;
            if(tmp == i){
                break;
            }
        }
        comp += 1;
    }
}

int main(int argc, char const *argv[]) {
    std::ios::sync_with_stdio(false);
    int n, m;
    cin >> n >> m;
    pop.resize(n);
    for(int i = 0; i < n; i += 1){
        pop[i].clear();
    }
    memset(dfn, 0, sizeof(dfn));
    memset(low, 0, sizeof(low));
    memset(ins, 0, sizeof(ins));
    memset(stack, 0, sizeof(stack));
    memset(belong, 0, sizeof(belong));
    memset(in_deg, 0, sizeof(in_deg));
    memset(out_deg, 0, sizeof(out_deg));
    for(int i = 0; i < m; i += 1){
        int a, b;
        cin >> a >> b;
        pop[b - 1].insert(a - 1);
    }

    for(int i = 0; i < n; i += 1){
        if(!dfn[i]){
            tarjan(i);
        }
    }

    for(int i = 0; i < n; i += 1){
        for(set<int>::iterator it=pop[i].begin(); it != pop[i].end(); it++){
            if(belong[i] != belong[*it]){
                in_deg[belong[*it]] += 1;
                out_deg[belong[i]] += 1;
            }
        }
    }
    int res = 0, idx = -1;
    for(int i = 0; i < comp; i += 1){
        if(in_deg[i] == 0){
            res += 1;
            idx = i;
        }
    }
    if(res != 1){
        cout << 0 << endl;
    }else{
        int res = 0;
        for(int i = 0; i < n; i += 1){
            if(belong[i] == idx){
                res += 1;
            }
        }
        cout << res << endl;
    }
    return 0;
}
