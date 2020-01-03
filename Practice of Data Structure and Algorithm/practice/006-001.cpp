#include <iostream>
using namespace std;

int n, m, s;
double v;

struct node{
    int from, to;
    double rate, comm;
}edges[256];

double dist[1024];
int edgesidx;

void addedge(int f, int t, double r, double c){
    edges[edgesidx].from = f;
    edges[edgesidx].to = t;
    edges[edgesidx].rate = r;
    edges[edgesidx].comm = c;
    edgesidx += 1;
    return ;
}

int bellman_ford(){
    for(int i = 1; i < n; i += 1){
        for(int j = 0; j < edgesidx; j += 1){
            if(dist[edges[j].to] < edges[j].rate * (dist[edges[j].from] - edges[j].comm)){
                dist[edges[j].to] = edges[j].rate * (dist[edges[j].from] - edges[j].comm);
            }
        }
    }
    for(int j = 0; j < edgesidx; j += 1){
        if(dist[edges[j].to] < edges[j].rate * (dist[edges[j].from] - edges[j].comm)){
            return 1;
        }
    }
    return 0;
}

int main(int argc, char const *argv[]) {
    cin >> n >> m >> s >> v;
    for(int i = 0; i < m; i += 1){
        int f, t;
        double r1, c1, r2, c2;
        cin >> f >> t >> r1 >> c1 >> r2 >> c2;
        addedge(f, t, r1, c1);
        addedge(t, f, r2, c2);
    }
    dist[s] = v;
    if(bellman_ford()){
        cout << "YES" << endl;
    }else{
        cout << "NO" << endl;
    }
    return 0;
}
