#include <iostream>
#include <vector>
#include <memory.h>
#include <queue>
using namespace std;

int n, m;
int infty = 1e5;

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

void add_edge(int from, int to, int capacity){
    edge.push_back(Edge(from, to, capacity, 0));
    edge.push_back(Edge(to, from, 0, 0));
    num_edges += 2;
    network[from].push_back(num_edges - 2);
    network[to].push_back(num_edges - 1);
}

int change[1024];
int idx[1024];

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


int main(int argc, char const *argv[]) {
    while(cin >> n >> m){
        num_edges = 0;
        network.clear();
        network.resize(n + m + 2);
        edge.clear();
        for(int i = 1; i <= n; i += 1){
            add_edge(0, i, 1);
            int num_stall = 0;
            cin >> num_stall;
            for(int j = 1; j <= num_stall; j += 1){
                int idx_stall;
                cin >> idx_stall;
                add_edge(i, idx_stall + n, 1);
            }
        }
        for(int i = 1; i <= m; i += 1){
            add_edge(n + i, n + m + 1, 1);
        }
        cout << max_flow(0, n + m + 1) << endl;
    }
    return 0;
}
