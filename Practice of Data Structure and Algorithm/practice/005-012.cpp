#include <iostream>
#include <vector>
#include <memory.h>
using namespace std;

#define clear(x) memset(x, 0, sizeof(x));

int low[1024], dfn[1024], ins[1024], stack[1024];
int depth, top;
vector<vector<int> > graph;
int n, m;
int num_group;

void tarjan(int idx){
    low[idx] = dfn[idx] = ++depth;
    ins[idx] = 1;
    stack[top++] = idx;
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
            if(tmp == idx){
                break;
            }
        }
        num_group += 1;
    }
    return ;
}

int main(int argc, char const *argv[]) {
    int numcases = 0;
    cin >> numcases;
    while(numcases--){
        cin >> n >> m;
        graph.resize(n);
        for(int i = 0; i < n; i += 1){
            graph[i].resize(0);
        }
        clear(low);
        clear(dfn);
        clear(ins);
        clear(stack);
        depth = top = 0;
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
        if(num_group == 1){
            cout << "Yes" << endl;
        }else{
            cout << "No" << endl;
        }
        num_group = 0;
    }
    return 0;
}
