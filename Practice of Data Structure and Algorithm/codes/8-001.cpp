#include <iostream>
using namespace std;

double dist[110] = {};
int num_currencies, num_exchange;
double src_currency;
int type_source;

struct Point{
    int src;
    int dst;
    double rate;
    double comm;
}point[210];

int point_idx = 0;

void add_point(int src, int dst, double rate, double comm){
    point[point_idx].src = src;
    point[point_idx].dst = dst;
    point[point_idx].rate = rate;
    point[point_idx].comm = comm;
    point_idx += 1;
    return ;
}

int bellman_ford(){
    for(int i = 1; i < num_currencies; i += 1){
        for(int j = 0; j < point_idx; j += 1){
            if(dist[point[j].dst] < (dist[point[j].src] - point[j].comm) * point[j].rate){
                dist[point[j].dst] = (dist[point[j].src] - point[j].comm) * point[j].rate;
            }
        }
    }
    for(int j = 0; j < point_idx; j += 1){
        if(dist[point[j].dst] < (dist[point[j].src] - point[j].comm) * point[j].rate){
            return 1;
        }
    }
    return 0;
}

int main(int argc, char const *argv[]) {
    cin >> num_currencies >> num_exchange >> type_source >> src_currency;
    int src, dst;
	double r1, c1, r2, c2;
    for(int i = 0; i < num_exchange; i += 1){
        cin >> src >> dst >> r1 >> c1 >> r2 >> c2;
        add_point(src, dst, r1, c1);
        add_point(dst, src, r2, c2);
    }
    for(int i = 1; i <= num_currencies; i += 1){
        dist[i] = 0;
    }
    dist[type_source] = src_currency;
    if(bellman_ford()){
        cout << "YES" << endl;
    }else{
        cout << "NO" << endl;
    }
    return 0;
}
