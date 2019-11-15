#include <iostream>
#include <vector>
#include <set>
#include <memory.h>
using namespace std;

int num_cases, n, m;
int dfn[1010], low[1010], ins[1010], stack[1010];
int num_group, depth, top;

vector<set<int> > room;

void tarjan(int i){
    stack[top++] = i;
    ins[i] = 1;
    dfn[i] = low[i] = ++depth;
    for(set<int>::iterator it = room[i].begin(); it != room[i].end(); it++){
        if(!dfn[*it]){
            tarjan(*it);
            low[i] = min(low[i], low[*it]);
        }else if(ins[*it]){
            low[i] = min(low[i], dfn[*it]);
        }
    }
    if(dfn[i] == low[i]){
        while(1){
            if(stack[--top] == i){
                break;
            }
        }
        num_group += 1;
    }
}

int main(int argc, char const *argv[]) {
	cin >> num_cases;
    while(num_cases--){
        cin >> n >> m;
        room.resize(n);
        for(int i = 0; i < n; i += 1){
            room[i].clear();
        }
        memset(dfn, 0, sizeof(dfn));
        memset(low, 0, sizeof(low));
        memset(ins, 0, sizeof(ins));
        memset(stack, 0, sizeof(stack));
        num_group = top = depth = 0;
        for(int i = 0; i < m; i += 1){
            int u, v;
            cin >> u >> v;
            room[u - 1].insert(v - 1);
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
