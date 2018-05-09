#include <iostream>
#include <vector>
#include <memory.h>
#include <queue>
using namespace std;

const int INF = 1000000000;
const int N = 128;

struct edge{
    int src;
    int dst;
    int cap;
    int flow;
    edge(int s, int d, int c, int f){
        src = s, dst = d, cap = c, flow = f;
    }
};

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
    int m, n;
    cin >> m >> n;
    int pigs[1024] = {};
    int vst[1024] = {};

    for(int i = 1; i <= m; i += 1){
        cin >> pigs[i];
        vst[i] = 0;
    }
    for(int i = 1; i <= n; i += 1){
        int k = 0;
        cin >> k;
        for(int j = 1; j <= k; j += 1){
            int tmp = 0;
            cin >> tmp;
            if(vst[tmp]){
                addedge(vst[tmp], i, INF);
            }else{
                addedge(0, i, pigs[tmp]);
                vst[tmp] = i;
            }
        }
        cin >> k;
        addedge(i, n + 1, k);
    }
    cout << maxflow(0, n + 1) << endl;
    return 0;
}
