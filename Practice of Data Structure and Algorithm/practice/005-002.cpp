#include <iostream>
#include <vector>
using namespace std;

vector<vector<int> > graph;
int ins[1024];
int stack[1024];
int low[1024];
int dfn[1024];
int top = 0;
int depth = 0;
int num_group = 0;
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
            if(tmp == idx){
                break;
            }
        }
        num_group += 1;
    }
}

int main(int argc, char const *argv[]) {
    int num_cases;
    cin >> num_cases;
    while(num_cases--){
        cin >> n >> m;
        top = 0;
        depth = 0;
        num_group = 0;
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
        if(num_group == 1){
            cout << "Yes" << endl;
        }else{
            cout << "No" << endl;
        }
    }
    return 0;
}
