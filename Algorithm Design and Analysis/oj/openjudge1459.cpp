#include <iostream>
#include <vector>
#include <memory.h>
#include <queue>
using namespace std;

const int INF = 1000000000;
const int N = 210;

struct edge{
    int src;
    int dst;
    int cap;
    int flow;
    edge(int s, int d, int c, int f){
        src = s, dst = d, cap = c, flow = f;
    }
};

int n = 0, m = 0;

vector<edge> e;
vector<int> g[N];

int a[N], p[N];

void init(){
    for(int i = 0; i <= N; i += 1){
        g[i].clear();
    }
    e.clear();
}

void addedge(int f, int t, int c){
    e.push_back(edge(f, t, c, 0));
    e.push_back(edge(t, f, 0, 0));
    g[f].push_back(e.size() - 2);
    g[t].push_back(e.size() - 1);
}

int maxflow(int src, int dst){
    int flow = 0;
    for( ; ; ){
        memset(a, 0, sizeof(a));
        queue<int> q;
        q.push(src);
        a[src] = INF;
        while(!q.empty()){
            int tmp = q.front();
            q.pop();
            for(int i = 0; i < g[tmp].size(); i += 1){
                edge& t_e = e[g[tmp][i]];
                if(!a[t_e.dst] && t_e.cap > t_e.flow){
                    p[t_e.dst] = g[tmp][i];
                    a[t_e.dst] = min(a[tmp], t_e.cap - t_e.flow);
                    q.push(t_e.dst);
                }
            }
            if(a[dst]){
                break;
            }
        }
        if(!a[dst]){
            break;
        }
        for(int u = dst; u != src; u = e[p[u]].src){
            e[p[u]].flow += a[dst];
            e[p[u] ^ 1].flow -= a[dst];
        }
        flow += a[dst];
    }
    return flow;
}

int main(){
    while(scanf("%d", &n) != EOF){
        init();
        int power, consumer, link;
        scanf("%d %d %d", &power, &consumer, &link);
        for(int i = 0; i < link; i += 1){
            int start, end, cost;
            scanf(" (%d,%d)%d", &start, &end, &cost);
            if(start == end){
                continue;
            }else{
                addedge(start, end, cost);
            }
        }
        for(int i = 0; i < power; i += 1){
            int end, cost;
            scanf(" (%d)%d", &end, &cost);
            addedge(n + 1, end, cost);
        }
        for(int i = 0; i < consumer; i += 1){
            int start, cost;
            scanf(" (%d)%d", &start, &cost);
            addedge(start, n + 2, cost);
        }
        printf("%d\n", maxflow(n + 1, n + 2));
    }
    return 0;
}
