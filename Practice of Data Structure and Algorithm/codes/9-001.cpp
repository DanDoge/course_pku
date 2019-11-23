#include <iostream>
#include <algorithm>
#include <vector>
#include <queue>
#include <memory.h>
using namespace std;


int from = 0;
int to;

struct Edge{
    int from;
    int to;
    int capacity;
    int flow;
    Edge(int a, int b, int c, int d){
        from = a;
        to = b;
        capacity = c;
        flow = d;
    }
};

vector<vector<int> > network;
vector<Edge> edge;

int num_edges;
int infty = 1e5;

void add_edge(int from, int to, int capacity){
    edge.push_back(Edge(from, to, capacity, 0));
    edge.push_back(Edge(to, from, 0, 0));
    num_edges += 2;
    network[from].push_back(num_edges - 2);
    network[to].push_back(num_edges - 1);
}

int change[128];
int idx[128];

int max_flow(int from, int to){
    int flow = 0;
    while(1){
        memset(change, 0, sizeof(change));
        queue<int> q;
        q.push(from);
        change[from] = infty;
        while(!q.empty()){
            int top = q.front();
            q.pop();
            for(int i = 0; i < network[top].size(); i += 1){
                Edge& e = edge[network[top][i]];
                if(!change[e.to] && e.capacity > e.flow){
                    idx[e.to] = network[top][i];
                    change[e.to] = min(change[top], e.capacity - e.flow);
                    q.push(e.to);
                }
            }
            if(change[to]){
                break;
            }
        }
        if(!change[to]){
            break;
        }

        for(int i = to; i != from; i = edge[idx[i]].from){
            edge[idx[i]].flow += change[to];
            edge[idx[i] ^ 1].flow -= change[to];
        }
        flow += change[to];
    }
    return flow;
}

struct Node{
    int ipt[16];
    int opt[16];
    int w;
}node[64];

int main(int argc, char const *argv[]) {
    int p, n;
    while(cin >> p >> n){
        network.resize(2 * n + 3);
        for(int i = 0; i < 2 * n + 3; i += 1){
            network[i].clear();
        }
        to = 2 * n + 1;
        for(int i = 1; i <= n; i += 1){
            cin >> node[i].w;
            int need_ipt = 0;
            for(int j = 0; j < p; j += 1){
                cin >> node[i].ipt[j];
                if(!need_ipt && node[i].ipt[j] == 1){
                    need_ipt = 1;
                }
            }
            if(!need_ipt){
                add_edge(from, i, infty);
            }
            int need_opt = 0;
            for(int j = 0; j < p; j += 1){
                cin >> node[i].opt[j];
                if(!need_opt && node[i].opt[j] == 0){
                    need_opt = 1;
                }
            }
            if(!need_opt){
                add_edge(n + i, to, infty);
            }
        }

        for(int i = 1; i <= n; i += 1){
            add_edge(i, n + i, node[i].w);
            for(int j = 1; j <= n; j += 1){
                if(i == j){

                }else{
                    int match = 1;
                    for(int k = 0; k < p; k += 1){
                        if(node[j].ipt[k] != 2 && node[i].opt[k] != node[j].ipt[k]){
                            match = 0;
                            break;
                        }
                    }
                    if(match){
                        add_edge(n + i, j, infty);
                    }
                }
            }
        }

        int ans = max_flow(from, to);
        int cnt = 0;
        int path[1024][4];
        for(int i = n + 1; i < to; i += 1){
            for(int j = 0; j < network[i].size(); j += 1){
                int v = network[i][j];
                Edge& e = edge[v];
                if(e.flow > 0 && e.to <= n){
                    path[cnt][0] = i - n;
                    path[cnt][1] = e.to;
                    path[cnt][2] = e.flow;
                    cnt += 1;
                }
            }
        }

        cout << ans << " " << cnt << endl;
        for(int i = 0; i < cnt; i += 1){
            cout << path[i][0] << " " << path[i][1] << " " << path[i][2] << endl;
        }
    }
    return 0;
}
