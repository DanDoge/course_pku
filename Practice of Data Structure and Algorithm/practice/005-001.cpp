#include <iostream>
#include <vector>
using namespace std;

int n, m;
vector<vector<int> > graph;
int stack[10010];
int ins[10010];
int low[10010];
int dfn[10010];
int group[10010];
int out_deg[10010];
int group_size[10010];
int group_idx = 1;
int dfn_top = 0;
int stack_top = 0;

void tarjan(int idx){
    low[idx] = dfn[idx] = ++dfn_top;
    ins[idx] = 1;
    stack[stack_top++] = idx;
    for(int i = 0; i < graph[idx].size(); i += 1){
        int j = graph[idx][i];
        if(!dfn[j]){
            tarjan(j);
            low[idx] = min(low[idx], low[j]);
        }else if(ins[j]){
            low[idx] = min(low[idx], dfn[j]);
        }
    }
    if(low[idx] == dfn[idx]){
        while(1){
            int tmp = stack[--stack_top];
            ins[tmp] = 0;
            group[tmp] = group_idx;
            if(tmp == idx){
                break;
            }
        }
        group_idx += 1;
    }
    return ;
}

int main(int argc, char const *argv[]) {
    cin >> n >> m;
    graph.resize(n + 1);
    for(int i = 1; i <= n; i += 1){
        graph[i].resize(0);
    }
    for(int i = 1; i <= m; i += 1){
        int from, to;
        cin >> from >> to;
        graph[from].push_back(to);
    }
    for(int i = 1; i <= n; i += 1){
        if(!dfn[i]){
            tarjan(i);
        }
    }
    for(int i = 1; i <= n; i += 1){
        //cout << group[i] << endl;
        group_size[group[i]] += 1;
        for(int j = 0; j < graph[i].size(); j += 1){
            if(group[i] != group[graph[i][j]]){
                out_deg[group[i]] += 1;
            }
        }
    }
    int res = 0, idx = -1;
    for(int i = 1; i < group_idx; i += 1){
        if(out_deg[i] == 0){
            res += 1;
            idx = i;
        }
    }
    if(res != 1){
        cout << 0 << endl;
    }else{
        cout << group_size[idx] << endl;
    }
    return 0;
}
