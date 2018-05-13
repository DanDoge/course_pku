#include <iostream>
#include <vector>
#include <memory.h>
#include <queue>
using namespace std;

const long long INF = ((long long)1) << 60;
const long long N = 1926;

struct edge{
    long long src;
    long long dst;
    long long cap;
    long long flow;
    edge(long long s, long long d, long long c, long long f){
        src = s, dst = d, cap = c, flow = f;
    }
};

long long f, path;

vector<edge> e;
vector<long long> g[N];

long long a[N], p[N];

void init(){
    for(long long i = 0; i <= N; i += 1){
        g[i].clear();
    }
    e.clear();
}

void addedge(long long f, long long t, long long c){
    e.push_back(edge(f, t, c, 0));
    e.push_back(edge(t, f, 0, 0));
    g[f].push_back(e.size() - 2);
    g[t].push_back(e.size() - 1);
}

long long maxflow(long long src, long long dst){
    long long flow = 0;
    for( ; ; ){
        memset(a, 0, sizeof(a));
        queue<long long> q;
        q.push(src);
        a[src] = INF;
        while(!q.empty()){
            long long tmp = q.front();
            q.pop();
            for(long long i = 0; i < g[tmp].size(); i += 1){
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
        for(long long u = dst; u != src; u = e[p[u]].src){
            e[p[u]].flow += a[dst];
            e[p[u] ^ 1].flow -= a[dst];
        }
        flow += a[dst];
    }
    return flow;
}

int main(){
    std::ios::sync_with_stdio(false);
    while(cin >> f >> path){
        long long farm_cows[256] = {};
        long long farm_cap[256]  = {};
        long long tot_cows = 0;
        for(long long i = 1; i <= f; i += 1){
            long long cows, cap;
            cin >> cows >> cap;
            farm_cows[i] = cows;
            tot_cows += cows;
            farm_cap[i]  = cap;
        }
        long long dis[256][256] = {};
        for(int i = 1; i <= f; i += 1){
            for(int j = 1; j<= f; j += 1){
                dis[i][j] = INF;
            }
            dis[i][i] = 0;
        }
        for(long long i = 1; i <= path; i += 1){
            long long src, dst; long long dist;
            cin >> src >> dst >> dist;
            dis[src][dst] = dis[dst][src] = min(dist, dis[dst][src]);
        }
        long long max_path = 0;
        for(long long k = 1; k <= f; k += 1){
            for(long long i = 1; i <= f; i += 1){
                for(long long j = 1; j <= f; j += 1){
                    dis[i][j] = min(dis[i][j], dis[i][k] + dis[k][j]);
                    max_path = max(max_path, dis[i][j]);
                }
            }
        }
        /*
        for(int i = 1; i <= f; i += 1){
            for(int j = 1; j<= f; j += 1){
                cout << dis[i][j] << ' ';
            }
            cout << endl;
        }
        */
        long long s = 1, e = max_path + 2;
        long long ans = -1;
        while(s <= e){
            long long mid = (s + e) / 2;
            init();
            for(long long i = 1; i <= f; i += 1){
                addedge(0, i, farm_cows[i]);
                addedge(i + f, 2 * f + 1, farm_cap[i]);
            }
            for(long long i = 1; i <= f; i += 1){
                for(long long j = 1; j <= f; j += 1){
                    if(dis[i][j] <= mid){
                        addedge(i, j + f, INF);
                    }
                }
            }
            if(maxflow(0, 2 * f + 1) == tot_cows){
                e = mid - 1;
                ans = mid;
            }else{
                s = mid + 1;
            }
        }
        cout << ans << endl;
    }
    return 0;
}
