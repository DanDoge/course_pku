#include <iostream>
#include <vector> 
using namespace std;

int low[10010], dfn[10010], stack[10010], ins[10010], group[10010], outdeg[10010];
int top = 0, depth = 0, groupidx = 0;
vector<vector<int> > graph;
int n, m;

void tarjan(int idx){
    ins[idx] = 1;
    stack[top++] = idx;
    low[idx] = dfn[idx] = ++depth;
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
            int tmp = stack[--top];
            ins[tmp] = 0;
            group[tmp] = groupidx;
            if(tmp == idx){
                break;
            }
        }
        groupidx += 1;
    }
}

int main(int argc, char const *argv[]) {
    cin >> n >> m;
    graph.resize(n);
    for(int i = 0; i < n; i += 1){
        graph[i].resize(0);
    }
    for(int i = 0; i < m; i += 1){
        int from, to;
        cin >> from >> to;
        graph[from - 1].push_back(to - 1);
    }
    for(int i = 0; i < n; i += 1){
        if(!dfn[i]){
            tarjan(i);
        }
    }
    for(int i = 0; i < n; i += 1){
        for(int j = 0; j < graph[i].size(); j += 1){
            int from = i;
            int to = graph[i][j];
            if(group[from] != group[to]){
                outdeg[group[from]] += 1;
            }
        }
    }
    int res = 0, idx = -1;
    for(int i = 0; i < groupidx; i += 1){
        if(outdeg[i] == 0){
            res += 1;
            idx = i;
        }
    }
    if(res != 1){
        cout << 0 << endl;
    }else{
        int cnt = 0;
        for(int i = 0; i < n; i += 1){
            cnt += group[i] == idx;
        }
        cout << cnt << endl;
    }
    return 0;
}
