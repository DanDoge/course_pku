#include <iostream>
#include <vector>
#include <memory.h>
#include <queue>
using namespace std;


int infty = 1e5;
int dis[256][256];
int k, c, m;

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

int change[256];
int idx[256];

int max_flow(int mid_dis){
    network.clear();
    edge.clear();
    num_edges = 0;
    network.resize(k + c + 2);
    for(int i = k + 1; i <= k + c; i += 1){
        add_edge(0, i, 1);
    }
    for(int i = 1; i <= k; i += 1){
        add_edge(i, k + c + 1, m);
    }
    for(int i = k + 1; i <= k + c; i += 1){
        for(int j = 1; j <= k; j += 1){
            if(dis[i][j] <= mid_dis){
                add_edge(i, j, 1);
            }
        }
    }
    int flow = 0;
    int from = 0;
    int to = k + c + 1;
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
    return flow == c;
}

void floyd(){
    for(int l = 1; l <= k + c; l += 1){
        for(int i = 1; i <= k + c; i += 1){
            for(int j = 1; j <= k + c; j += 1){
                dis[i][j] = min(dis[i][k] + dis[k][j], dis[i][j]);
            }
        }
    }
}

int main(int argc, char const *argv[]) {
    while(cin >> k >> c >> m){
        for(int i = 1; i <= k + c; i += 1){
            for(int j = 1; j <= k + c; j += 1){
                cin >> dis[i][j];
                if(dis[i][j] == 0){
                    dis[i][j] = infty;
                }
            }
        }
        floyd();
        int right_dis = 10010;
        int left_dis = 0;
        while(left_dis < right_dis){
            int mid_dis = (left_dis + right_dis) / 2;
            if(max_flow(mid_dis)){
                right_dis = mid_dis;
            }else{
                left_dis = mid_dis + 1;
            }
        }
        cout << right_dis << endl;
    }
    return 0;
}
