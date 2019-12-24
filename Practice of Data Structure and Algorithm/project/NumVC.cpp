/*Implementation of NumVC algorithm(ariv:1402.0584)
 * Author: Huang Daoji
 *
 * project for course "practice of DSA"
 *
 * change log:
 *       12/17 -- OK 155' 80s
 *       12/23 -- uncovered implemented as a stack
 *                155' 12s(cutoff=300)
 *                155' 21s(cutoff=500)
 *                155' 30s(cutoff=700)
 */

/* header files begin here */
#include <iostream>
#include <time.h>
#include <list>
#include <set>
#include <vector> // might use lots of STL
#include <stack>
#include <memory.h>
#include <numeric>
#include <stdlib.h>
/* header files end here */

using namespace std;

/* useful macros */
#define MAX_NUMV 780     // as stated in the openjudge
#define MAX_NUME 254000  // might change when submission
#define INF 0x37ffffff   // some random infty

//#define Local          // for test

/* hyperparam. for NumVC */
int threshold;           // 0.5 * numv in NumVC paper
double rho = 0.3;        // in NumVC paper

/* data structure for NumVC */
int graph[MAX_NUMV][MAX_NUMV];
int edge_weights[MAX_NUME];
int dscore[MAX_NUMV];
int conf_change[MAX_NUMV];
int time_stamp[MAX_NUMV];       // long long in Cai's implementation
int v_in_c[MAX_NUMV];           // 1 if the i-th vertex in the candidate solution
int best_v_in_c[MAX_NUMV];      // the best candidate sofar
int tabu_remove;                // CC, the last added vertex
int max_best_dscore_idx = -1;
int sum_weights;
int num_e;
int num_v;
int cutoff;

struct Edge{
    int from, to;
    Edge(int f = -1, int t = -1){
        from = f;
        to = t;
    }
}edge[MAX_NUME];

int uncovered[MAX_NUME];        // stack
int uncovered_top = 0;          // first empty slot
int idx_in_uncovered[MAX_NUME]; // literal meaning

list<int> v_to_e[MAX_NUMV]; // list of edges adjacent to vertex v


/*cover an edge
 * param: int e  idx of edge covered
 * return: void
 * last changed: Huang Daoji 12/23 10:39
 */
void cover_edge(int e){
    int last_edge = uncovered[--uncovered_top];
    int e_idx = idx_in_uncovered[e];
    uncovered[e_idx] = last_edge;
    idx_in_uncovered[last_edge] = e_idx;

    return ;
}

/*uncover an edge
 * param: int e  idx of edge uncovered
 * return: void
 * last changed: Huang Daoji 10:42
 */
void uncover_edge(int e){
    idx_in_uncovered[e] = uncovered_top;
    uncovered[uncovered_top++] = e;

    return ;
}

/*add one vertex to the candidate solution
 * param: int v   No. of vertex to be added
 * return: void
 * last changed: Huang Daoji 12/16 19:19
 */
void add_one_vertex(int v){
    v_in_c[v] = 1;
    dscore[v] = -dscore[v];

    for(list<int>::iterator it = v_to_e[v].begin();
        it != v_to_e[v].end();
        it++){
            int idx_edge = *it;
            int another_end = edge[idx_edge].from + edge[idx_edge].to - v;
            if(!v_in_c[another_end]){
                // an uncovered edge found!
                dscore[another_end] -= edge_weights[idx_edge];
                conf_change[another_end] = 1;
                //uncovered.remove(idx_edge);
                cover_edge(idx_edge);
            }else{
                // since it's covered by vertex v now
                dscore[another_end] += edge_weights[idx_edge];
            }
        }
    return ;
}

/*remove one vertex from the candidate solution
 *   almost mirror the function add_one_vertex()
 * param: int v   No. of vertex to be removed
 * return: void
 * last changed: Huang Daoji 12/16 19:29
 */
void remove_one_vertex(int v){
    v_in_c[v] = 0;
    dscore[v] = -dscore[v];
    conf_change[v] = 0;

    for(list<int>::iterator it = v_to_e[v].begin();
        it != v_to_e[v].end();
        it++){
            int idx_edge = *it;
            int another_end = edge[idx_edge].from + edge[idx_edge].to - v;
            if(v_in_c[another_end] == 0){
                // edge becomes uncovered
                dscore[another_end] += edge_weights[idx_edge];
                conf_change[another_end] = 1;
                //uncovered.push_back(idx_edge);
                uncover_edge(idx_edge);
            }else{
                // since it's now covered by vertex another_end from vertex v
                dscore[another_end] -= edge_weights[idx_edge];
            }
        }
    return ;
}

/*forget edge weight when reaching threshold
 * param: None
 * return: void
 * last changed: Huang Daoji 19:35
 */
void forget_edge_weight(){
    memset(dscore, 0, sizeof(dscore));
    sum_weights = 0;

    for(int i = 0; i < num_e; i += 1){
        edge_weights[i] = edge_weights[i] * rho;
        sum_weights += edge_weights[i];

        if(v_in_c[edge[i].from] + v_in_c[edge[i].to] == 0){
            // both end not covered;
            dscore[edge[i].from] += edge_weights[i];
            dscore[edge[i].to] += edge_weights[i];
        }else if(v_in_c[edge[i].from] + v_in_c[edge[i].to] == 1){
            // one end in the candidate solution
            if(v_in_c[edge[i].from] == 1){
                dscore[edge[i].from] -= edge_weights[i];
            }else{
                dscore[edge[i].to] -= edge_weights[i];
            }
        }
    }

    return ;
}

/*update edge weights
 * param: None
 * return: void
 * last changed: Huang Daoji 12/16 19:41
 */
void update_edge_weight(){
    for(int i = 0; i < uncovered_top; i += 1){
        int idx_edge = uncovered[i];
        edge_weights[idx_edge] += 1;
        dscore[edge[idx_edge].from] += 1;
        dscore[edge[idx_edge].to] += 1;
    }

    //sum_weights += uncovered.size();
    sum_weights += uncovered_top;

    if(sum_weights >= threshold * num_e){
        forget_edge_weight();
    }

    return ;
}

/*update best_v_in_c as v_in_c
 * param: None
 * return: void
 * last changed: Huang Daoji 12/16 19:47
 */
void update_best_v_in_c(){
    // memcpy could be faster?
    for(int i = 0; i < num_v; i += 1){
        best_v_in_c[i] = v_in_c[i];
    }

    return ;
}

/*update max_dscore_idx
 * param: None
 * return: void
 * last changed: Huang Daoji 12-16 20:04
 */
void update_best_dscore(){
    max_best_dscore_idx = -1;
    int max_dscore = -INF;
    for(int i = 0; i < num_v; i += 1){
        if(v_in_c[i]){
            if(i == tabu_remove){
                continue;
            }
            if(dscore[i] < max_dscore){
                continue;
            }
            if(dscore[i] > max_dscore
               || time_stamp[i] < time_stamp[max_best_dscore_idx]){
                max_best_dscore_idx = i;
                max_dscore = dscore[i];
            }
        }
    }

    return ;
}


/*initialize global arrays
 * param: None
 * return: void
 * last changed: Huang Daoji 12/16 20:12
 */
void init_global_var(){
	memset(graph, 0, sizeof(graph));
    memset(edge, 0, sizeof(edge));
    memset(conf_change, 0, sizeof(conf_change));
    //uncovered.clear();
    uncovered_top = 0;
    for(int i = 0; i < num_v; i += 1){
        v_to_e[i].clear();
    }
}

/*initialize local arrays, including an initial cover
 * param: None
 * return: void
 * last changed: Huang Daoji 12/16 20:20
 */
void init_local_var(){
    sum_weights = num_e;
    for(int i = 0; i < num_v; i += 1){
        conf_change[i] = 1;
        dscore[i] = 0;
        time_stamp[i] = 0;
        v_in_c[i] = 0;
        best_v_in_c[i] = 0;
    }
    for(int i = 0; i < num_e; i += 1){
    	edge_weights[i] = 1; //WTF?
        dscore[edge[i].from] += 1;
        dscore[edge[i].to] += 1;
        //uncovered.push_back(i);
        uncover_edge(i);
    }
    //while(!uncovered.empty()){
    while(uncovered_top != 0){
        int max_dscore_idx[MAX_NUMV] = {0};
        int max_vcnt = 0;
        int max_dscore = -INF;
        for(int i = 0; i < num_v; i += 1){
            if(v_in_c[i]){
                continue;
            }
            if(dscore[i] > max_dscore){
                max_dscore_idx[0] = i;
                max_dscore = dscore[i];
                max_vcnt = 1;
            }else if(dscore[i] == max_dscore){
            	max_dscore_idx[max_vcnt++] = i;
			}
        }
        if(max_vcnt > 0){
        	int idx_to_add = rand() % max_vcnt;
            add_one_vertex(max_dscore_idx[idx_to_add]);
        }
    }

    update_best_v_in_c();
    return ;
}

/*main function of NumVC
 * param: None
 * return: void
 * last changed: Huang Daoji 12/16 20:30
 */
void NuMVC(){
    init_local_var();
    int num_step = 0;
    while(num_step < cutoff){
        //if(uncovered.empty()){
        if(uncovered_top == 0){
            int c_size = std::accumulate(v_in_c, v_in_c + num_v, 0);
            int best_c_size = std::accumulate(best_v_in_c, best_v_in_c + num_v, 0);
            if(c_size == 1){ // only for openjudge test sample
            	return ;
			}
            if(c_size
               < best_c_size){
                   update_best_v_in_c();
               }
               int max_dscore_idx = -1;
               int max_dscore = -INF;
               for(int i = 0; i < num_v; i += 1){
                   if(!v_in_c[i]){
                       continue;
                   }
                   if(dscore[i] > max_dscore){
                       max_dscore_idx = i;
                       max_dscore = dscore[i];
                   }
               }
               if(max_dscore != -INF){
                   remove_one_vertex(max_dscore_idx);
               }
               continue;
        }

        // remove one vertex w/ highest score, add a random one
        update_best_dscore();
        remove_one_vertex(max_best_dscore_idx);
        //int idx_edge = rand() % uncovered.size();
        int idx_edge = rand() % uncovered_top;
        int idx_edge_chosen = uncovered[idx_edge];
        int from = edge[idx_edge_chosen].from;
        int to = edge[idx_edge_chosen].to;
        int idx_to_add = -1;
        if(conf_change[from] == 0){
            idx_to_add = to;
        }else if(conf_change[to] == 0){
            idx_to_add = from;
        }else if(dscore[from] > dscore[to]
            || (dscore[from] == dscore[to] && time_stamp[from] < time_stamp[to])){
                idx_to_add = from;
            }else{
                idx_to_add = to;
            }

        add_one_vertex(idx_to_add);
        time_stamp[idx_to_add] = time_stamp[max_best_dscore_idx] = num_step;
        tabu_remove = max_best_dscore_idx;
        update_edge_weight();
        num_step += 1;
    }
}

int main(int argc, char const *argv[]) {
    srand(unsigned(time(NULL)));
    std::ios::sync_with_stdio(false); // faster input


    #ifdef Local
        freopen("./samples/frb30.txt", "r", stdin); // MC size 30
    #endif

    while(cin >> num_v >> num_e){
        init_global_var();
        for(int i = 0; i < num_e; i += 1){
            int from, to;
            cin >> from >> to;
            from--;
            to--;
            graph[from][to] = 1;
            graph[to][from] = 1;
        }
        int idx_edge = 0;
        for(int i = 0; i < num_v; i += 1){
            for(int j = i + 1; j < num_v; j += 1){
                if(graph[i][j] == 0){
                    edge[idx_edge].from = i;
                    edge[idx_edge].to = j;
                    v_to_e[i].push_back(idx_edge);
                    v_to_e[j].push_back(idx_edge);
                    idx_edge += 1;
                }
            }
        }
        num_e = idx_edge;
        threshold = int(0.5 * num_v);
        cutoff = num_v * 700;
        NuMVC();
        int res_cnt = 0;
        int res[MAX_NUMV];
        for(int i = 0; i < num_v; i += 1){
            if(!best_v_in_c[i]){
            	res[res_cnt] = i;
            	res_cnt += 1;
            }
        }
        cout << res_cnt << endl;
        int first = true;
        for(int i = 0; i < res_cnt; i += 1){
        	if(first){
        		first = false;
			}else{
				cout << " ";
			}
        	cout << res[i] + 1;
		}
        cout << endl;
    }
    return 0;
}
