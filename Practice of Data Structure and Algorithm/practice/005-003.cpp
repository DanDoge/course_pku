#include <iostream>
#include <vector>
#include <memory.h>
#include <algorithm>
using namespace std;

int low[1024];
int dfn[1024];
int cut[1024];
int top = 0;
int depth = 0;
vector<int> res;
vector<vector<int> > graph;

void tarjan(int idx, int fa){
    low[idx] = dfn[idx] = ++depth;
    int num_child = 0;
    for(int i = 0; i < graph[idx].size(); i += 1){
        int j = graph[idx][i];
        if(!dfn[j]){
            num_child += 1;
            tarjan(j, idx);
            low[idx] = min(low[idx], low[j]);
            if(low[j] >= dfn[idx]){
                cut[idx] += 1;
            }
        }else if(dfn[j] < dfn[idx] && j != fa){
        	//cout << "!" << endl;
            low[idx] = min(low[idx], dfn[j]);
        }
    }
    //cout << idx << " " << cut[idx] << " " << low[idx] << " " << dfn[idx] << " " << fa << endl;
    if(fa == -1){
        if(num_child >= 2){
            res.push_back(idx);
        }
    }else{
        if(cut[idx] >= 1){
            res.push_back(idx);
            cut[idx] += 1;
        }
    }
}

int main(int argc, char const *argv[]) {
    int caseidx = 1;
    while(1){
        int from, to;
        cin >> from;
        if(from == 0){
            break;
        }else{
            graph.resize(1024);
            res.resize(0);
            for(int i = 0; i < 1024; i += 1){
                graph[i].resize(0);
            }
            memset(low, 0, sizeof(low));
            memset(dfn, 0, sizeof(dfn));
            memset(cut, 0, sizeof(cut));
            top = 0;
            depth = 0;

            if(caseidx != 1){
                cout << endl;
            }
            cout << "Network #" << caseidx++ << endl;
            int max_idx = -1;
            do{
                cin >> to;
                //cout << from << "!" << to << endl;
                max_idx = max(max_idx, to);
                max_idx = max(max_idx, from);
                graph[from].push_back(to);
                graph[to].push_back(from);
                cin >> from;
            }while(from != 0);
            for(int i = 1; i <= max_idx; i += 1){
                if(!dfn[i]){
                    tarjan(i, -1);
                }
            }
            sort(res.begin(), res.end());
            for(int i = 0; i < res.size(); i += 1){
                cout << "  SPF node " << res[i] << " leaves " << cut[res[i]] << " subnets" << endl;
            }
            if(res.size() == 0){
            	cout << "  No SPF nodes" << endl;
			}
        }
    }
    return 0;
}
